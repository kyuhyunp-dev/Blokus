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
        throw std::runtime_error("[GameServer] Failed to bind to port " + std::to_string(ServerPort));
    }

    mSelector.add(mListener);
    
    spdlog::info("[GameServer] Server started. Listening on port {}...", ServerPort);
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
            // Always check clients
            handleIncomingPackets();
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
            spdlog::info("[GameServer] New client connected from {}", address.value().toString());
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
            spdlog::info("[GameServer] Handle Incoming Packet");
            sf::Packet packet;
            sf::Socket::Status status = client.receive(packet);

            if (status == sf::Socket::Status::Done) 
            {
                spdlog::info("[GameServer] Received a packet from a client!");
                processPacket(packet, client); 
            } 
            else if (status == sf::Socket::Status::Disconnected || status == sf::Socket::Status::Error) 
            {
                spdlog::info("[GameServer] Client disconnected.");

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
                    spdlog::info("[GameServer] Removed player from match {}. Remaining: {}", matchCode, match.players.size());

                    mSocketToMatch.erase(itMatch);
                    
                    if (match.players.empty())
                    {
                        spdlog::info("[GameServer] Match {} is empty and has been destroyed.", matchCode);
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
                spdlog::info("[GameServer] Processing CreateMatch request...");
                
                if (mSocketToMatch.find(&sender) != mSocketToMatch.end())
                {
                    spdlog::warn("[GameServer] NACK: Client attempted to create a match but is already in one.");

                    std::string code = mSocketToMatch[&sender];
                    sf::Packet createPacket = formMatchJoinFailedPacket(NetworkProtocol::JoinError::AlreadyInMatch);
                    sendMatchJoinFailedPacket(sender, createPacket, code, NetworkProtocol::JoinError::AlreadyInMatch);
                    break; 
                } 

                std::string newCode = generateMatchCode();
                              
                Server::Player player { UNDECIDED, &sender }; 
                Server::Match match { newCode, { player } };
                mMatches[newCode] = match;

                // Register in the reverse-lookup map for quick disconnects
                mSocketToMatch[&sender] = newCode;

                sf::Packet createPacket = formMatchJoinedPacket(newCode);
                sendMatchJoinedPacket(sender, createPacket, newCode);
                break;
            }
            case NetworkProtocol::PacketType::JoinMatch:
            {
                spdlog::info("[GameServer] Processing JoinMatch request...");

                NetworkProtocol::JoinMatchRequest request;
                packet >> request;
                
                if (mSocketToMatch.find(&sender) != mSocketToMatch.end())
                {
                    sf::Packet joinPacket = formMatchJoinFailedPacket(NetworkProtocol::JoinError::AlreadyInMatch);
                    sendMatchJoinFailedPacket(sender, joinPacket, request.matchCode, NetworkProtocol::JoinError::AlreadyInMatch);
                    break;
                }

                auto matchIt = mMatches.find(request.matchCode);
                if (matchIt != mMatches.end())
                {
                    Server::Match& match = matchIt->second;
                    if (match.players.size() >= MAX_PLAYER)
                    {
                        sf::Packet joinPacket = formMatchJoinFailedPacket(NetworkProtocol::JoinError::MatchFull);
                        sendMatchJoinFailedPacket(sender, joinPacket, request.matchCode, NetworkProtocol::JoinError::MatchFull);
                        break;
                    }

                    Server::Player player { UNDECIDED, &sender };
                    match.players.push_back(player);

                    mSocketToMatch[&sender] = request.matchCode;
                    
                    sf::Packet joinPacket = formMatchJoinedPacket(request.matchCode);
                    sendMatchJoinedPacket(sender, joinPacket, request.matchCode);
                }
                else 
                {
                    sf::Packet joinPacket = formMatchJoinFailedPacket(NetworkProtocol::JoinError::MatchNotFound);
                    sendMatchJoinFailedPacket(sender, joinPacket, request.matchCode, NetworkProtocol::JoinError::MatchNotFound);
                }
                break;
            }
            default:
            {
                spdlog::warn("[GameServer] Received unexpected packet type on server.");
                break;
            }
        } 
    }
}

// A quick helper to translate the enum to a readable string for the logs
constexpr std::string_view getErrorString(NetworkProtocol::JoinError reason)
{
    switch (reason)
    {
        case NetworkProtocol::JoinError::MatchFull:       return "Match Full";
        case NetworkProtocol::JoinError::MatchNotFound:   return "Match Not Found";
        case NetworkProtocol::JoinError::AlreadyInMatch:  return "Already In Match";
        default:                                          return "Unknown Error";
    }
}
sf::Packet GameServer::formMatchJoinedPacket(std::string_view code)
{
    sf::Packet packet;
    NetworkProtocol::MatchJoinedResponse response { std::string(code) };
    packet << NetworkProtocol::PacketType::MatchJoined << response;
    return packet;
}

sf::Packet GameServer::formMatchJoinFailedPacket(NetworkProtocol::JoinError reason)
{
    sf::Packet packet;
    NetworkProtocol::MatchJoinFailedResponse response { reason };
    packet << NetworkProtocol::PacketType::MatchJoinFailed << response;
    return packet;
}

void GameServer::sendMatchJoinedPacket(sf::TcpSocket& client, sf::Packet& packet, std::string_view code)
{
    if (sendToClient(client, packet) == sf::Socket::Status::Done)
    {
        spdlog::info("[GameServer] ACK: Client successfully assigned to match with Code: {}", code);
    }
    else
    {
        spdlog::error("[GameServer] NACK: Failed to send MatchJoined packet to client for Code: {}", code);
    }
}

void GameServer::sendMatchJoinFailedPacket(sf::TcpSocket& client, sf::Packet& packet, std::string_view code, NetworkProtocol::JoinError reason)
{
    if (sendToClient(client, packet) == sf::Socket::Status::Done)
    {
        spdlog::warn("[GameServer] NACK: Match join rejected for Code '{}'. Reason: {}", code, getErrorString(reason));
    }
    else
    {
        spdlog::error("[GameServer] NACK: Failed to send MatchJoinFailed packet to client. Code: '{}', Reason: {}", code, getErrorString(reason));
    }
}

sf::Socket::Status GameServer::sendToClient(sf::TcpSocket& client, sf::Packet& packet)
{
    return client.send(packet);
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