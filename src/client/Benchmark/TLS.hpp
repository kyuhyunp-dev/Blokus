#include <iostream>
#include <chrono>
#include <vector>
#include <numeric>
#include <algorithm>
#include <thread>
#include <SFML/Network.hpp>
#include "shared/Network/NetworkProtocol.hpp"
#include <sys/resource.h>
#include <spdlog/spdlog.h>


// For Server
void runHandshakeBenchmark(bool isNative) 
{
    const int NUM_REQUESTS = 1000;
    std::vector<long long> rttMeasurements;
    rttMeasurements.reserve(NUM_REQUESTS);

    // Port 8081 for Stunnel Proxy, Port 443 (or direct server port) for Native TLS
    const unsigned short port = isNative ? 443 : 8081;
    const std::string modeName = isNative ? "Native TLS" : "Stunnel Proxy";

    spdlog::info("Starting Pure TLS Handshake Benchmark [{}] ({} requests)...", modeName, NUM_REQUESTS);

    for (int i = 0; i < NUM_REQUESTS; ++i) 
    {
        auto start = std::chrono::steady_clock::now();

        sf::TcpSocket socket;
        
        // 1. Establish TCP connection
        if (socket.connect(sf::IpAddress::LocalHost, port) != sf::Socket::Status::Done) 
        {
            spdlog::error("Failed to connect [{}] on port {} at iteration {}", modeName, port, i);
            break;
        }

        // 2. Perform Native TLS Client Setup if enabled
        if (isNative)
        {
            sf::TcpSocket::TlsStatus tlsStatus = socket.setupTlsClient("localhost", false);
            if (tlsStatus != sf::TcpSocket::TlsStatus::HandshakeComplete)
            {
                spdlog::error("Native TLS Handshake failed at iteration {}", i);
                break;
            } 
        }

        auto end = std::chrono::steady_clock::now();
        
        // Immediately disconnect to reset for next connection attempt
        socket.disconnect();

        long long rttUs = std::chrono::duration_cast<std::chrono::microseconds>(end - start).count();
        rttMeasurements.push_back(rttUs);

        std::this_thread::sleep_for(std::chrono::milliseconds(5));
    }

    if (rttMeasurements.empty()) return;

    std::sort(rttMeasurements.begin(), rttMeasurements.end());
    long long total = std::accumulate(rttMeasurements.begin(), rttMeasurements.end(), 0LL);
    long long average = total / rttMeasurements.size();
    long long medianUs = rttMeasurements[rttMeasurements.size() / 2];

    spdlog::info("--- Handshake Benchmark Results [{}] ---", modeName);
    spdlog::info("Total Connections: {}", rttMeasurements.size());
    spdlog::info("Average Handshake: {} us ({:.3f} ms)", average, average / 1000.0);
    spdlog::info("Median Handshake:  {} us ({:.3f} ms)", medianUs, medianUs / 1000.0);
    spdlog::info("Min Handshake:     {} us ({:.3f} ms)", rttMeasurements.front(), rttMeasurements.front() / 1000.0);
    spdlog::info("Max Handshake:     {} us ({:.3f} ms)", rttMeasurements.back(), rttMeasurements.back() / 1000.0);
}

// For Client
void runPerPacketRTTBenchmark(bool isNative) 
{
    sf::TcpSocket socket;
    const unsigned short port = isNative ? 443 : 8081;
    const std::string modeName = isNative ? "Native TLS" : "Stunnel Proxy";

    if (socket.connect(sf::IpAddress::LocalHost, port) != sf::Socket::Status::Done) 
    {
        spdlog::error("Failed to connect [{}] on port {}", modeName, port);
        return;
    }

    if (isNative)
    {
        sf::TcpSocket::TlsStatus tlsStatus = socket.setupTlsClient("localhost", false);
        if (tlsStatus != sf::TcpSocket::TlsStatus::HandshakeComplete)
        {
            spdlog::error("Native TLS Handshake failed on initial socket connection");
            return;
        } 
    }

    const int NUM_REQUESTS = 1000;
    std::vector<long long> rttMeasurements;
    rttMeasurements.reserve(NUM_REQUESTS);
    
    spdlog::info("Starting Per-Packet RTT Benchmark [{}] ({} requests)...", modeName, NUM_REQUESTS);

    for (int i = 0; i < NUM_REQUESTS; ++i) 
    {
        sf::Packet request;
        request << NetworkProtocol::PacketType::JoinMatch 
                << NetworkProtocol::JoinMatchRequest{"9999"};

        auto start = std::chrono::steady_clock::now();

        if (socket.send(request) != sf::Socket::Status::Done) 
        {
            spdlog::error("Failed to send packet at iteration {}", i);
            break;
        }

        sf::Packet response;
        if (socket.receive(response) != sf::Socket::Status::Done) 
        {
            spdlog::error("Failed to receive packet at iteration {}", i);
            break;
        }

        auto end = std::chrono::steady_clock::now();

        long long rttUs = std::chrono::duration_cast<std::chrono::microseconds>(end - start).count();
        rttMeasurements.push_back(rttUs);

        std::this_thread::sleep_for(std::chrono::milliseconds(5));
    }

    if (rttMeasurements.empty()) 
    {
        spdlog::error("No measurements collected.");
        return;
    }

    std::sort(rttMeasurements.begin(), rttMeasurements.end());
    long long total = std::accumulate(rttMeasurements.begin(), rttMeasurements.end(), 0LL);
    long long average = total / rttMeasurements.size();
    long long medianUs = rttMeasurements[rttMeasurements.size() / 2];

    spdlog::info("--- Per-Packet RTT Benchmark Results [{}] ---", modeName);
    spdlog::info("Total Requests: {}", rttMeasurements.size());
    spdlog::info("Average RTT:    {} us ({:.3f} ms)", average, average / 1000.0);
    spdlog::info("Median RTT:     {} us ({:.3f} ms)", medianUs, medianUs / 1000.0);
    spdlog::info("Min RTT:        {} us ({:.3f} ms)", rttMeasurements.front(), rttMeasurements.front() / 1000.0);
    spdlog::info("Max RTT:        {} us ({:.3f} ms)", rttMeasurements.back(), rttMeasurements.back() / 1000.0);
}

void printPeakMemoryUsage() 
{
    struct rusage usage;
    if (getrusage(RUSAGE_SELF, &usage) == 0) 
    {
#if defined(__APPLE__)
        // macOS returns bytes
        double rssMb = usage.ru_maxrss / (1024.0 * 1024.0);
        spdlog::info("Peak Process RSS: {} Bytes ({:.2f} MiB)", usage.ru_maxrss, rssMb);
#else
        // Linux returns kilobytes
        double rssMb = usage.ru_maxrss / 1024.0;
        spdlog::info("Peak Process RSS: {} KB ({:.2f} MiB)", usage.ru_maxrss, rssMb);
#endif
    }
}