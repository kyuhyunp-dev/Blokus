#ifndef MOCK_TCPSOCKET_HPP
#define MOCK_TCPSOCKET_HPP

#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include <SFML/Network/TcpSocket.hpp>

using ::testing::_;
using ::testing::Return;
using ::testing::DoAll;
using ::testing::SetArgReferee;

class MockTcpSocket : public sf::TcpSocket 
{
public:
    MOCK_METHOD(sf::Socket::Status, connect, (const sf::IpAddress&, unsigned short, sf::Time));
    MOCK_METHOD(void, disconnect, ());
    MOCK_METHOD(void, setBlocking, (bool));
    MOCK_METHOD(sf::TcpSocket::TlsStatus, setupTlsClient, (const std::string&, bool));
    MOCK_METHOD(sf::Socket::Status, send, (sf::Packet&));
    MOCK_METHOD(sf::Socket::Status, receive, (sf::Packet&));
};

#endif