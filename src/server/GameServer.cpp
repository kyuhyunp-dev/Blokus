#include "GameServer.hpp"
#include "shared/Network/NetworkProtocol.hpp"
#include <spdlog/spdlog.h>
#include <random>
#include <algorithm>


GameServer::GameServer() 
    : mIsRunning(true) 
{
    if (mListener.listen(ServerPort) != sf::Socket::Status::Done) 
    {
        throw std::runtime_error("Failed to bind to port " + std::to_string(ServerPort));
    }

    mSelector.add(mListener);
    
    spdlog::info("Server started. Listening on port {}...", ServerPort);
}

void GameServer::run() 
{
    while (mIsRunning) 
    {
        // wait() puts the server to sleep until network activity occurs
        if (mSelector.wait()) 
        {
            // Someone wants to join
            if (mSelector.isReady(mListener)) 
            {
                handleIncomingConnections();
            }
            // Currently connected client has sent data
            else 
            {
                handleIncomingPackets();
            }
        }
    }
}

void GameServer::handleIncomingConnections() 
{
    auto client = std::make_unique<sf::TcpSocket>();
    
    if (mListener.accept(*client) == sf::Socket::Status::Done) 
    {
        auto address = client->getRemoteAddress();
        if (address.has_value())
        {
            spdlog::info("New client connected from {}", address.value().toString());
        }

        mSelector.add(*client);
        mClients.push_back(std::move(client));
    }
}

void GameServer::handleIncomingPackets() 
{
    for (auto it = mClients.begin(); it != mClients.end(); ) 
    {
        sf::TcpSocket& client = **it;
        
        if (mSelector.isReady(client)) 
        {

            spdlog::info("Handle Incoming Packet");
            sf::Packet packet;
            sf::Socket::Status status = client.receive(packet);

            if (status == sf::Socket::Status::Done) 
            {
                spdlog::info("Received a packet from a client!");
                processPacket(packet, client); 
            } 
            else if (status == sf::Socket::Status::Disconnected || status == sf::Socket::Status::Error) 
            {
                spdlog::info("Client disconnected.");

                auto itMatch = mSocketToMatch.find(&client);
                if (itMatch != mSocketToMatch.end()) 
                {
                    std::string matchCode = itMatch->second;
                    auto& match = mMatches[matchCode];
                    
                    match.players.erase(
                        std::remove_if(match.players.begin(), match.players.end(),
                        [&client](const Server::Player& player) 
                        { return player.socketPtr == &client; }),
                        match.players.end()
                    );
                    spdlog::info("Removed player from match {}. Remaining: {}", matchCode, match.players.size());

                    mSocketToMatch.erase(itMatch);
                    
                    if (match.players.empty())
                    {
                        spdlog::info("Match {} is empty and has been destroyed.", matchCode);
                        mMatches.erase(matchCode);
                    }
                }

                mSelector.remove(client);
                it = mClients.erase(it); 

                continue;
            } 
        } 

        ++it;
    }
}

void GameServer::processPacket(sf::Packet& packet, sf::TcpSocket& sender)
{
    NetworkProtocol::PacketType type;
    sf::Packet responsePacket;
    if (packet >> type)
    {
        switch(type) 
        {
            case NetworkProtocol::PacketType::CreateMatch:
            {
                spdlog::info("Processing CreateMatch request...");

                std::string newCode = generateMatchCode();
                
                Server::Player player { UNDECIDED, &sender }; 
                Server::Match match { newCode, { player } };
                mMatches[newCode] = match;

                // Register in the reverse-lookup map for quick disconnects
                mSocketToMatch[&sender] = newCode;

                std::string reason = fmt::format("Match {} successfully created for host.", newCode);
                formMatchJoinedPacket(newCode);
                sendMatchJoinedPacket(sender, reason);
                break;
            }
            case NetworkProtocol::PacketType::JoinMatch:
            {
                spdlog::info("Processing JoinMatch request...");

                NetworkProtocol::JoinMatchRequest request;
                packet >> request;

                auto matchIt = mMatches.find(request.matchCode);
                if (matchIt != mMatches.end())
                {
                    Server::Match& match = matchIt->second;
                    if (match.players.size() >= MAX_PLAYER)
                    {
                        std::string reason = fmt::format("Failed JoinMatch request with Code {}: Too Many Players", request.matchCode);
                        formMatchJoinFailedPacket(reason);
                        sendMatchJoinFailedPacket(sender, reason);
                        break;
                    }

                    Server::Player player { UNDECIDED, &sender };
                    match.players.push_back(player);

                    mSocketToMatch[&sender] = request.matchCode;
                    
                    std::string reason = fmt::format("Guest successfully joined match with Code {}", request.matchCode);
                    formMatchJoinedPacket(request.matchCode);
                    sendMatchJoinedPacket(sender, reason);
                }
                else 
                {
                    std::string reason = fmt::format("Failed JoinMatch request with Code {}: Code not found", request.matchCode);
                    formMatchJoinFailedPacket(reason);
                    sendMatchJoinFailedPacket(sender, reason);
                }
                break;
            }
            default:
            {
                spdlog::warn("Received unexpected packet type on server.");
                break;
            }
        } 
    }
}

void GameServer::formMatchJoinedPacket(const std::string& code)
{
    mOutboundPacket.clear();
    NetworkProtocol::MatchJoinedResponse response { code };
    mOutboundPacket << NetworkProtocol::PacketType::MatchJoined << response;
}

void GameServer::formMatchJoinFailedPacket(const std::string& reason)
{
    mOutboundPacket.clear();
    NetworkProtocol::MatchJoinFailedResponse response { reason };
    mOutboundPacket << NetworkProtocol::PacketType::MatchJoinFailed << response;
}

void GameServer::sendMatchJoinedPacket(sf::TcpSocket& client, const std::string& logContext)
{
    if (client.send(mOutboundPacket) == sf::Socket::Status::Done)
    {
        spdlog::info(logContext);
    }
    else
    {
        spdlog::error("Failed to send MatchJoined packet to client. {}", logContext);
    }
}

void GameServer::sendMatchJoinFailedPacket(sf::TcpSocket& client, const std::string& logContext)
{
    if (client.send(mOutboundPacket) == sf::Socket::Status::Done)
    {
        spdlog::warn(logContext);
    }
    else
    {
        spdlog::error("Failed to send MatchJoinFailed packet to client. {}", logContext);
    }
}

std::string GameServer::generateMatchCode()
{
    // Explicit charset eliminates ASCII gaps and confusing characters (like O vs 0, I vs 1)
    static const char alphabet[] = "ABCDEFGHJKLMNPQRSTUVWXYZ23456789";
    static const size_t alphabetSize = sizeof(alphabet) - 1;

    // Static engines are initialized exactly once, preserving entropy and speed
    static std::random_device rd;
    static std::mt19937 gen(rd());
    std::uniform_int_distribution<size_t> distrib(0, alphabetSize - 1);

    std::string code(CODE_DIGITS, ' ');

    do 
    {
        for (int i = 0; i < CODE_DIGITS; ++i)
        {
            code[i] = alphabet[distrib(gen)];
        }
    } 
    while (mMatches.find(code) != mMatches.end()); 
    return code;
}