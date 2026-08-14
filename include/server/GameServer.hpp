#ifndef GAME_SERVER_HPP
#define GAME_SERVER_HPP

#include "server/DataTypes/Client.hpp"
#include "server/DataTypes/Match.hpp"
#include "shared/Network/NetworkProtocol.hpp"

#include <SFML/Network/TcpSocket.hpp>
#include <SFML/Network/SocketSelector.hpp>
#include <SFML/Network/TcpListener.hpp>
#include <vector>
#include <string>
#include <map>
#include <memory>
#include <unordered_map>


namespace sf 
{
    class Packet;
}

class GameServer 
{
public:
    GameServer(unsigned short port = ServerPort);
    void run();

protected: // for run and unit test
    void handleIncomingConnections();
    void handleIncomingPackets();
    
    void flushOutgoingQueues();
    void removeClient(uint32_t clientID);

    // for unit test
    std::unordered_map<uint32_t, Server::Client> mClients;
    std::unordered_map<sf::TcpSocket*, uint32_t> mSocketToID;
    std::map<std::string, Server::Match> mMatches;
    std::vector<uint32_t> mClientsToDisconnect;

    sf::SocketSelector mSelector;
    sf::TcpListener mListener;

private:
    void processPacket(sf::Packet& packet, uint32_t clientID);
    void queueMatchJoinedPacket(uint32_t clientID, sf::Packet& packet, std::string_view code);
    void queueMatchJoinFailedPacket(uint32_t clientID, sf::Packet& packet, NetworkProtocol::JoinError reason);
    void enqueuePacket(uint32_t clientID, sf::Packet& packet);

    void handleTitleStatePackets(sf::Packet& packet, Server::Client& client);
    void handleUsernameStatePackets(sf::Packet& packet, Server::Client& client);

    std::string_view getJoinErrorString(NetworkProtocol::JoinError reason);

    sf::Packet formMatchJoinedPacket(std::string_view code);
    sf::Packet formMatchJoinFailedPacket(NetworkProtocol::JoinError reason);

    std::string generateMatchCode();

private:
    static constexpr int CODE_DIGITS = 4;
    static constexpr int MAX_PLAYER = 2;

    bool mIsRunning;
    uint32_t mNextClientID = 0;
};

#endif