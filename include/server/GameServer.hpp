#ifndef GAME_SERVER_HPP
#define GAME_SERVER_HPP

#include "server/Match.hpp"
#include <SFML/Network/TcpSocket.hpp>
#include <SFML/Network/SocketSelector.hpp>
#include <SFML/Network/TcpListener.hpp>
#include <vector>
#include <string>
#include <map>
#include <memory>

namespace sf 
{
    class Packet;
}

class GameServer 
{
public:
    GameServer();
    void run();

protected: // for run and unit test
    void handleIncomingConnections();
    void handleIncomingPackets();
    void processPacket(sf::Packet& packet, sf::TcpSocket& sender);

    // for unit test
    std::vector<std::unique_ptr<sf::TcpSocket>> mClients;
    std::map<sf::TcpSocket*, std::string> mSocketToMatch; 
    std::map<std::string, Server::Match> mMatches;
    sf::SocketSelector mSelector;

protected:
    virtual sf::Socket::Status sendToClient(sf::TcpSocket& client, sf::Packet& packet);

private:
    sf::Packet formMatchJoinedPacket(std::string_view code);
    sf::Packet formMatchJoinFailedPacket(NetworkProtocol::JoinError reason);

    void sendMatchJoinedPacket(sf::TcpSocket& client, sf::Packet& packet, std::string_view code);
    void sendMatchJoinFailedPacket(sf::TcpSocket& client, sf::Packet& packet, std::string_view code, NetworkProtocol::JoinError reason);
   
    std::string generateMatchCode();

private:
    static constexpr int UNDECIDED = -1;
    static constexpr int CODE_DIGITS = 4;
    static constexpr int MAX_PLAYER = 2;

    sf::TcpListener mListener;

    bool mIsRunning;
};

#endif