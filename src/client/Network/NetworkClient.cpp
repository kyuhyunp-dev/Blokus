#include "Network/NetworkClient.hpp"
#include <SFML/Network/Dns.hpp>
#include <SFML/Network/IpAddress.hpp>
#include <spdlog/spdlog.h>
#include <chrono>


NetworkClient::NetworkClient()
    : mSocket()
    , mIsConnected(false)
    , mPingClock()
{
}

bool NetworkClient::connect(std::string_view ip, unsigned short port, 
    std::string_view tlsHostname, bool verifyPeer)
{
    // In SFML 3.x, IP addresses must be safely resolved
    auto address = sf::Dns::resolve(ip);
    if (!address || (*address).empty()) 
    {
        spdlog::error("[NetworkClient] Failed to resolve IP address: {}", ip);        
        return false;
    }

    // Ensure socket is blocking during the initial connection handshake
    mSocket.setBlocking(true);

    // Attempt to connect with a 5-second timeout
    sf::Socket::Status status = mSocket.connect((*address).front(), port, sf::seconds(5.f));

    if (status == sf::Socket::Status::Done)
    {
        spdlog::info("[NetworkClient] TCP successfully connected to {}:{}", tlsHostname, port);

        sf::TcpSocket::TlsStatus tlsStatus = mSocket.setupTlsClient(tlsHostname, verifyPeer);
        if (tlsStatus != sf::TcpSocket::TlsStatus::HandshakeComplete)
        {
            spdlog::error("[NetworkClient] TLS Handshake failed for hostname: {}", tlsHostname);
            mSocket.disconnect();
            return false;
        }

        mIsConnected = true; 

        // Set to non-blocking for continuous pollEvent 
        mSocket.setBlocking(false); 

        spdlog::info("[NetworkClient] Successfully established TLS connection to {}:{}", tlsHostname, port);
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