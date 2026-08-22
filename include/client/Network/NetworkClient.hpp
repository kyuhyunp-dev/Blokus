#ifndef NETWORK_CLIENT_HPP
#define NETWORK_CLIENT_HPP

#include <SFML/Network/TcpSocket.hpp>
#include <SFML/Network/Packet.hpp>
#include <SFML/Network/Socket.hpp>
#include <SFML/System/Clock.hpp>

class NetworkClient 
{
public:
    NetworkClient();
    virtual ~NetworkClient() = default; 

    virtual bool connect(std::string_view ip, unsigned short port);
    void disconnect();
    
    // Core communication mechanics
    virtual void sendPacket(sf::Packet& packet);
    virtual bool pollPacket(sf::Packet& outPacket); // Non-blocking read
    
    virtual bool isConnected() const;

private:
    sf::TcpSocket mSocket;         // The actual data pipe
    bool          mIsConnected;    // Track if our pipe is open
    sf::Clock     mPingClock;      // Useful if you ever add basic heartbeat checks

};

#endif