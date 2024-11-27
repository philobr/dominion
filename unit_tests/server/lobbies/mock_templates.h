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
                (std::unique_ptr<shared::ServerToClientMessage> message, const shared::PlayerBase::id_t &player_id),
                (override));
};

MATCHER(IsCreateLobbyResponseMessage, "Checks if the message is CreateLobbyResponseMessage")
{
    auto create_lobby_response_msg = dynamic_cast<shared::CreateLobbyResponseMessage *>(arg.get());
    return create_lobby_response_msg != nullptr;
}

MATCHER(IsSuccessMessage, "Checks if the message is a success message")
{
    auto result_msg = dynamic_cast<shared::ResultResponseMessage *>(arg.get());
    return (result_msg != nullptr) && result_msg->success;
}

MATCHER(IsFailureMessage, "Checks if the message is a failure message")
{
    auto result_msg = dynamic_cast<shared::ResultResponseMessage *>(arg.get());
    return (result_msg != nullptr) && !result_msg->success;
}

MATCHER(IsJoinLobbyBroadcastMessage, "Checks if the message is JoinLobbyBroadcastMessage")
{
    auto join_lobby_broadcast_msg = dynamic_cast<shared::JoinLobbyBroadcastMessage *>(arg.get());
    return join_lobby_broadcast_msg != nullptr;
}
