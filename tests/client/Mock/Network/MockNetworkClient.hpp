#ifndef MOCK_NETWORKCLIENT_HPP
#define MOCK_NETWORKCLIENT_HPP

#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include "States/TitleState.hpp"

#include "Mock/Resource/MockTextureHolder.hpp"
#include "Mock/Resource/MockFontHolder.hpp"

// Assuming you have a MockNetworkClient. If not, this is what it should look like:
class MockNetworkClient : public NetworkClient 
{
public:
    MOCK_METHOD(bool, isConnected, (), (const, override));
    MOCK_METHOD(bool, connect, (std::string_view ip, unsigned short), (override));
    MOCK_METHOD(void, sendPacket, (sf::Packet&), (override));
    MOCK_METHOD(bool, pollPacket, (sf::Packet&), (override));
};

#endif
