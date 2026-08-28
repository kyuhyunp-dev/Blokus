#ifndef NETWORK_CLIENT_HPP
#define NETWORK_CLIENT_HPP

#include <SFML/Network/TcpSocket.hpp>
#include <SFML/Network/Packet.hpp>
#include <SFML/Network/SocketSelector.hpp>
#include <SFML/Network/Dns.hpp>
#include <SFML/System/Clock.hpp>
#include <spdlog/spdlog.h>
#include <chrono>


template <typename SocketType = sf::TcpSocket>
class NetworkClientImpl 
{
public:
    NetworkClientImpl()
    : mSocket()
    , mIsConnected(false)
    , mPingClock()
    {
    }

    virtual ~NetworkClientImpl() = default; 

    virtual bool connect(std::string_view ip, unsigned short port, std::string_view tlsHostname, bool verifyPeer)
    {
        // In SFML 3.x, IP addresses must be safely resolved
        auto address = sf::Dns::resolve(ip);
        if (!address || address->empty()) 
        {
            spdlog::error("[NetworkClient] Failed to resolve IP address: {}", ip);        
            mIsConnected = false;
            return false;
        }

        // Ensure socket is blocking during the initial connection handshake
        mSocket.setBlocking(true);

        if (mSocket.connect(address->front(), port, sf::seconds(5.f)) != sf::Socket::Status::Done)
        {
            mIsConnected = false;
            return false;
        }

        mSocket.setBlocking(false);

        const auto deadline = std::chrono::steady_clock::now() + std::chrono::seconds(5);
        sf::SocketSelector selector;
        selector.add(mSocket);

        sf::TcpSocket::TlsStatus tlsStatus = mSocket.setupTlsClient(std::string(tlsHostname), verifyPeer);
        while (std::chrono::steady_clock::now() < deadline)
        {
            if (tlsStatus == sf::TcpSocket::TlsStatus::HandshakeComplete)
            {
                spdlog::info("[NetworkClient] TCP successfully connected to {}:{}", tlsHostname, port);
                mIsConnected = true;
                return true;
            }
            else if (tlsStatus != sf::TcpSocket::TlsStatus::HandshakeStarted)
            {
                spdlog::error("[NetworkClient] TLS Handshake failed for hostname: {}", tlsHostname);
                mIsConnected = false;
                mSocket.disconnect();
                return false;
            }

            if (selector.wait(sf::milliseconds(50)))
            {
                tlsStatus = mSocket.setupTlsClient(std::string(tlsHostname), verifyPeer);
            }
        }

        spdlog::error("[NetworkClient] Failed to connect to {}:{}", ip, port);        
        mIsConnected = false;
        mSocket.disconnect();
        return false;
    }

    void disconnect()
    {
        if (mIsConnected)
        {
            mSocket.disconnect();
            mIsConnected = false;

            spdlog::warn("[NetworkClient] Disconnected from server.");    
        }
    }
    
    // Core communication mechanics
    virtual void sendPacket(sf::Packet& packet)
    {
        if (!mIsConnected)
        {
            return;
        }

        mOutgoingQueue.push_back(packet);
        flushOutgoingQueue();
    }

    virtual bool pollPacket(sf::Packet& outPacket)
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
            mSocket.disconnect();
            spdlog::warn("[NetworkClient] Disconnected by server during poll.");
        }

        return false;
    }

    virtual bool isConnected() const
    {
        return mIsConnected;
    }

    SocketType& getSocket() { return mSocket; }

    void flushOutgoingQueue()
    {
        if (!mIsConnected)
        { 
            return;
        }
        

        while (!mOutgoingQueue.empty())
        {
            sf::Packet& packet = mOutgoingQueue.front();
            sf::Socket::Status status = mSocket.send(packet);

            if (status == sf::Socket::Status::Done)
            {
                mOutgoingQueue.pop_front(); // Successfully sent
            }
            else if (status == sf::Socket::Status::Partial || status == sf::Socket::Status::NotReady)
            {
                // Socket buffer is full or blocked. Keep packet at front and retry next tick.
                break;
            }
            else
            {
                // Disconnected or Error
                mIsConnected = false;
                mSocket.disconnect();
                mOutgoingQueue.clear();
                spdlog::warn("[NetworkClient] Lost connection during packet send.");
                break;
            }
        }
    }
    
private:
    SocketType mSocket;         // The actual data pipe
    bool mIsConnected;    // Track if our pipe is open
    sf::Clock mPingClock;      // Useful if you ever add basic heartbeat checks
    std::deque<sf::Packet> mOutgoingQueue;
};

using NetworkClient = NetworkClientImpl<sf::TcpSocket>;

#endif