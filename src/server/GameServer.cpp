#include "GameServer.hpp"
#include <spdlog/spdlog.h>
#include <random>
#include <algorithm>


GameServer::GameServer(unsigned short port) 
    : mIsRunning(true) 
    , mNextClientID(0)
    , mClientsToDisconnect()
{
    if (mListener.listen(port) != sf::Socket::Status::Done) 
    {
        throw std::runtime_error("[GameServer] Failed to bind to port " + std::to_string(port));
    }

    // Preventing Phantom Connection
    mListener.setBlocking(false);
    mSelector.add(mListener);
    
    spdlog::info("[GameServer] Server started. Listening on port {}...", port);
}

void GameServer::run() 
{
    while (mIsRunning) 
    { // waits with a fixed timeout
        sf::Time timeout = sf::milliseconds(50);
        if (mSelector.wait(timeout)) 
        { 
            if (mSelector.isReady(mListener)) 
            {
                handleIncomingConnections();
            }
            // Always check clients
            handleIncomingPackets();
        }

        flushOutgoingQueues();

        for (uint32_t idToRemove: mClientsToDisconnect)
        {   
            removeClient(idToRemove);
        }
        mClientsToDisconnect.clear();
    }
}

void GameServer::handleIncomingConnections() 
{
    auto client = std::make_unique<sf::TcpSocket>();
    
    spdlog::info("[GameServer] New Client {}", static_cast<void*>(client.get()));
 
    if (mListener.accept(*client) == sf::Socket::Status::Done) 
    {
        client->setBlocking(false);

        auto address = client->getRemoteAddress();
        if (address.has_value())
        {
            mSelector.add(*client);
            
            uint32_t newID = mNextClientID++;
            mSocketToID[client.get()] = newID;
            mClients[newID].id = newID;
            mClients[newID].socket = std::move(client);
            mClients[newID].state = Server::ClientState::TitleState;

            spdlog::info("[GameServer] New Client {} connected from {}", newID, address.value().toString());
        }
    }
}

void GameServer::handleIncomingPackets() 
{
    constexpr int MAX_PACKETS_PER_TICK = 20;
    constexpr std::size_t MAX_PACKET_BYTES = 512; 

    for (const auto& [id, client]: mClients)
    {
        if (mSelector.isReady(*client.socket))
        {
            spdlog::info("[GameServer] Handle Incoming Packet for Client {}", id);
            int packetsReceivedThisTick = 0;
            sf::Packet packet;
            
            while (true) 
            { // Drain the socket until empty OR rate limit is hit
                sf::Socket::Status status = (*client.socket).receive(packet);

                if (status == sf::Socket::Status::Done) 
                {
                    spdlog::info("[GameServer] Client {} received packet", id);
                    packetsReceivedThisTick++;
                    
                    if (packetsReceivedThisTick > MAX_PACKETS_PER_TICK)
                    {
                        spdlog::warn("[GameServer] Client {} exceeded rate limit ({} packets). Dropping.", id, MAX_PACKETS_PER_TICK);
                        mClientsToDisconnect.push_back(id);
                        break; // Stop draining, they are dead to us
                    }

                    if (packet.getDataSize() > MAX_PACKET_BYTES)
                    {
                        spdlog::warn("[GameServer] Client {} sent oversized packet ({} bytes). Dropping.", id, packet.getDataSize());
                        mClientsToDisconnect.push_back(id);
                        break;
                    }

                    processPacket(packet, id); 
                }
                else if (status == sf::Socket::Status::Disconnected || status == sf::Socket::Status::Error) 
                {
                    spdlog::info("[GameServer] Client {} disconnected.", id);
                    mClientsToDisconnect.push_back(id);
                    break; 
                }
                else
                { // Socket is fully drained for this tick, move to the next client
                    spdlog::info("[GameServer] Socket is fully drained for Client {}", id);
                    break;
                }
            }
        }
    }
}

void GameServer::processPacket(sf::Packet& packet, uint32_t clientID)
{
    auto it = mClients.find(clientID);
    if (it == mClients.end())
    {
        spdlog::warn("[GameServer] Client {} sent a malformed packet. Dropping.", clientID);
        return;
    }
    
    spdlog::info("[GameServer] Client {} processing packet.", clientID);
    auto& client = it->second;
    switch(client.state)
    {
        case Server::ClientState::TitleState:
            spdlog::info("[GameServer] Processing TitleState request for Client {}...", client.id);
            handleTitleStatePackets(packet, client);
            break;
        case Server::ClientState::UsernameState:
            spdlog::info("[GameServer] Processing UsernameState request for Client {}...", client.id);
            handleUsernameStatePackets(packet, client);
            break;
        default:
            spdlog::warn("Client {} sent an invalid packet", clientID);
            mClientsToDisconnect.push_back(clientID); 
            break;
    }
}

void GameServer::handleTitleStatePackets(sf::Packet& packet, Server::Client& client)
{
    NetworkProtocol::PacketType type;
    if (!(packet >> type)) 
    {
        spdlog::warn("[GameServer] Client {} sent a malformed packet. Dropping.", client.id);
        mClientsToDisconnect.push_back(client.id);
        return;
    }

    switch (type)
    {
        case NetworkProtocol::PacketType::CreateMatch:
        {
            if (!packet.endOfPacket()) 
            {
                spdlog::warn("[GameServer] Client {} sent a malformed packet. Dropping.", client.id);
                mClientsToDisconnect.push_back(client.id);
                return; 
            }

            spdlog::info("[GameServer] Processing CreateMatch request for Client {}...", client.id);

            if (client.currentMatchCode)
            { // Match already exists
                std::string matchCode = *client.currentMatchCode;

                // State consistency (Partially created or partially deleted a match)
                if (mMatches.find(matchCode) == mMatches.end()) {
                    spdlog::error("State corruption: Client {} in missing match {}", client.id, matchCode);
                    mClientsToDisconnect.push_back(client.id);
                    return;
                }

                sf::Packet createPacket = formMatchJoinFailedPacket(NetworkProtocol::JoinError::AlreadyInMatch);
                queueMatchJoinFailedPacket(client.id, createPacket, NetworkProtocol::JoinError::AlreadyInMatch);
                break; 
            }
            
            std::string newCode = generateMatchCode();
            client.currentMatchCode = newCode;                            

            Server::Match match { newCode, { client.id }};
            mMatches[newCode] = match;

            sf::Packet createPacket = formMatchJoinedPacket(newCode);
            queueMatchJoinedPacket(client.id, createPacket, newCode);
            client.state = Server::ClientState::UsernameState;
            break;
        }
        case NetworkProtocol::PacketType::JoinMatch:
        {

            NetworkProtocol::JoinMatchRequest request;
            if (!(packet >> request) || !packet.endOfPacket())
            {
                spdlog::warn("[GameServer] Client {} sent a malformed packet. Dropping.", client.id);
                mClientsToDisconnect.push_back(client.id);
                return;
            }

            spdlog::info("[GameServer] Processing JoinMatch request for Client {}...", client.id);

            if (client.currentMatchCode)
            {
                sf::Packet joinPacket = formMatchJoinFailedPacket(NetworkProtocol::JoinError::AlreadyInMatch); 
                queueMatchJoinFailedPacket(client.id, joinPacket, NetworkProtocol::JoinError::AlreadyInMatch);
                break; 
            }

            std::string matchCode = request.matchCode;

            auto matchIt = mMatches.find(matchCode);
            if (matchIt == mMatches.end())
            { // matches already holds user's match code
                sf::Packet joinPacket = formMatchJoinFailedPacket(NetworkProtocol::JoinError::MatchNotFound);
                queueMatchJoinFailedPacket(client.id, joinPacket, NetworkProtocol::JoinError::MatchNotFound);
                break;
            }

            Server::Match& match = matchIt->second;
            if (match.playerIDs.size() >= MAX_PLAYER)
            {
                sf::Packet joinPacket = formMatchJoinFailedPacket(NetworkProtocol::JoinError::MatchFull);
                queueMatchJoinFailedPacket(client.id, joinPacket, NetworkProtocol::JoinError::MatchFull);
                break;
            }

            client.currentMatchCode = matchCode;
            match.playerIDs.push_back(client.id);

            sf::Packet joinPacket = formMatchJoinedPacket(matchCode);
            queueMatchJoinedPacket(client.id, joinPacket, matchCode);
            client.state = Server::ClientState::UsernameState;
            break;
        }
        default:
            spdlog::warn("Client {} sent invalid packet for TitleScreen", client.id);
            mClientsToDisconnect.push_back(client.id);
            break;
    }
}

void GameServer::handleUsernameStatePackets(sf::Packet& packet, Server::Client& client)
{

}

void GameServer::removeClient(uint32_t clientID)
{
    auto it = mClients.find(clientID);
    if (it == mClients.end()) 
    {
        spdlog::warn("[GameServer] Client {} sent a malformed packet. Dropping.", clientID);
        return;
    }

    auto& client = it->second;
    spdlog::info("[GameServer] Removing client {}", clientID);

    if (client.currentMatchCode.has_value())
    {
        std::string matchCode = client.currentMatchCode.value();
        auto matchIt = mMatches.find(matchCode);
        if (matchIt == mMatches.end())
        {
            spdlog::warn("[GameServer] Match {} does not exist.", matchCode);
            return;
        }

        auto& match = matchIt->second;
        match.playerIDs.erase(
            std::remove(match.playerIDs.begin(), match.playerIDs.end(), clientID),
            match.playerIDs.end()
        );
        
        if (match.playerIDs.empty())
        {
            mMatches.erase(matchCode);
        }
    }

    mSelector.remove(*client.socket);
    mSocketToID.erase(client.socket.get());
    
    mClients.erase(it);
}

std::string_view GameServer::getJoinErrorString(NetworkProtocol::JoinError reason)
{
    switch (reason)
    {
        case NetworkProtocol::JoinError::MatchFull:
            return "Match Full";
        case NetworkProtocol::JoinError::MatchNotFound:
            return "Match Not Found";
        case NetworkProtocol::JoinError::AlreadyInMatch:
            return "Already In Match";
        default:
            return "Unknown Error";
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

void GameServer::flushOutgoingQueues()
{
    for (auto& [id, client] : mClients) 
    {
        while (!client.outgoingQueue.empty()) 
        {
            sf::Packet& packet = client.outgoingQueue.front();
            sf::Socket::Status status = client.socket->send(packet);
            
            if (status == sf::Socket::Status::Done) 
            { // Successful send
                client.outgoingQueue.pop_front(); 
            } 
            else if (status == sf::Socket::Status::Partial) 
            { // The OS buffer is full. Stop trying to send for this client on this tick
                break; 
            }
            else 
            { // Handle Error / Disconnect
                mClientsToDisconnect.push_back(id);
                break;
            }
        }
    }
}

void GameServer::queueMatchJoinedPacket(uint32_t clientID, sf::Packet& packet, std::string_view code)
{
    enqueuePacket(clientID, packet);
    spdlog::info("[GameServer] Queued MatchJoined for Client {} (Code: {})", clientID, code);
}

void GameServer::queueMatchJoinFailedPacket(uint32_t clientID, sf::Packet& packet, NetworkProtocol::JoinError reason)
{
    enqueuePacket(clientID, packet);
    spdlog::warn("[GameServer] Queued MatchJoinFailed for Client {}. Reason: {}", clientID, getJoinErrorString(reason));
}

void GameServer::enqueuePacket(uint32_t clientID, sf::Packet& packet)
{
    auto it = mClients.find(clientID);
    if (it != mClients.end())
    {
        it->second.outgoingQueue.push_back(packet);
    }
}

std::string GameServer::generateMatchCode()
{
    // Explicit charset eliminates ASCII gaps and confusing characters (like O vs 0, I vs 1)
    static const char alphabet[] = "ABCDEFGHJKLMNPQRSTUVWXYZ23456789";
    static const size_t alphabetSize = sizeof(alphabet) - 1;

    // Static engines are initialized exactly once, preserving entropy and speed
    static thread_local std::random_device rd;
    static thread_local std::mt19937 gen(rd());
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
