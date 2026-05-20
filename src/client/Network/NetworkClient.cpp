#include "Network/NetworkClient.hpp"


NetworkClient::NetworkClient()
{

}

bool NetworkClient::connect(const std::string& ip, unsigned short port)
{
    return true;
}

void NetworkClient::disconnect()
{

}

// Core communication mechanics
void NetworkClient::sendPacket(sf::Packet& packet)
{

}

bool NetworkClient::pollPacket(sf::Packet& outPacket)
{
    return true;    
} // Non-blocking read

bool NetworkClient::isConnected() const
{
    return mIsConnected;
}