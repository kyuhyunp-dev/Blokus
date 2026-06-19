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
    sf::Packet mOutboundPacket;   
    std::map<sf::TcpSocket*, std::string> mSocketToMatch; 
    std::map<std::string, Server::Match> mMatches;
    sf::SocketSelector mSelector;

private:
    void formMatchJoinedPacket(const std::string& code);
    void formMatchJoinFailedPacket(const std::string& reason);
    
    void sendMatchJoinedPacket(sf::TcpSocket& client, const std::string& logContext);
    void sendMatchJoinFailedPacket(sf::TcpSocket& client, const std::string& logContext);

    std::string generateMatchCode();

private:
    static constexpr int UNDECIDED = -1;
    static constexpr int CODE_DIGITS = 4;
    static constexpr int MAX_PLAYER = 2;

    sf::TcpListener mListener;

    bool mIsRunning;
};

#endif