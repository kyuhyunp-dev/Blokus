#include "Network/NetworkClient.hpp"
#include <SFML/Network/IpAddress.hpp>
#include <spdlog/spdlog.h>


NetworkClient::NetworkClient()
    : mSocket()
    , mIsConnected(false)
    , mPingClock()
{
}

bool NetworkClient::connect(const std::string& ip, unsigned short port)
{
    // In SFML 3.x, IP addresses must be safely resolved
    std::optional<sf::IpAddress> address = sf::IpAddress::resolve(ip);
    if (!address) 
    {
        spdlog::error("[NetworkClient] Failed to resolve IP address: {}", ip);        
        return false;
    }

    // Ensure socket is blocking during the initial connection handshake
    mSocket.setBlocking(true);

    // Attempt to connect with a 5-second timeout
    sf::Socket::Status status = mSocket.connect(address.value(), port, sf::seconds(5.f));

    if (status == sf::Socket::Status::Done)
    {
        mIsConnected = true;
        
        // Set to non-blocking for continuous pollEvent 
        mSocket.setBlocking(false); 
        
        spdlog::info("[NetworkClient] Successfully connected to {}:{}", ip, port);
        return true;
    }
    else
    {
        mIsConnected = false;
        
        spdlog::error("[NetworkClient] Failed to connect to {}:{}", ip, port);        
        return false;
    }
}

void NetworkClient::disconnect()
{ // For client intentionally leaving the game
    if (mIsConnected)
    {
        mSocket.disconnect();
        mIsConnected = false;

        spdlog::warn("[NetworkClient] Disconnected from server.");    
    }
}

void NetworkClient::sendPacket(sf::Packet& packet)
{
    if (!mIsConnected)
    {
        return;
    }

    sf::Socket::Status status = mSocket.send(packet);

    if (status == sf::Socket::Status::Disconnected)
    {
        mIsConnected = false;

        spdlog::warn("[NetworkClient] Lost connection while sending packet.");
    }
}

bool NetworkClient::pollPacket(sf::Packet& outPacket)
{
    if (!mIsConnected) 
    {
        return false;
    }

    sf::Socket::Status status = mSocket.receive(outPacket);

    if (status == sf::Socket::Status::Done)
    {
        return true;
    }
    else if (status == sf::Socket::Status::Disconnected)
    {
        mIsConnected = false;

        spdlog::warn("[NetworkClient] Disconnected by server during poll.");
    }

    return false;
}

bool NetworkClient::isConnected() const
{
    return mIsConnected;
}