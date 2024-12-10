#pragma once
#include <server/network/message_interface.h>

#include <gmock/gmock.h>
#include <gtest/gtest.h>

using ::testing::_;
using ::testing::InSequence;
using ::testing::Truly;

/**
 * @brief Mock implementation of the MessageInterface
 *
 * @details Used for testing the game logic without actually sending messages to the client
 */
class MockMessageInterface : public server::MessageInterface
{
public:
    // Mock the sendMessage method, assuming it takes these parameters
    MOCK_METHOD(void, sendMessage,
                (const shared::ServerToClientMessage &message, const shared::PlayerBase::id_t &player_id), (override));
};

MATCHER(IsCreateLobbyResponseMessage, "Checks if the message is CreateLobbyResponseMessage")
{
    return typeid(arg) == typeid(const shared::CreateLobbyResponseMessage &);
}

MATCHER(IsSuccessMessage, "Checks if the message is a success message")
{
    return typeid(arg) == typeid(const shared::ResultResponseMessage &) &&
            dynamic_cast<const shared::ResultResponseMessage &>(arg).success;
}

MATCHER(IsFailureMessage, "Checks if the message is a failure message")
{
    return typeid(arg) == typeid(const shared::ResultResponseMessage &) &&
            !dynamic_cast<const shared::ResultResponseMessage &>(arg).success;
}

MATCHER(IsJoinLobbyBroadcastMessage, "Checks if the message is JoinLobbyBroadcastMessage")
{
    return typeid(arg) == typeid(const shared::JoinLobbyBroadcastMessage &);
}
