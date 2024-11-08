#include <server/lobbies.h>
#include <server/message_interface.h>
#include <shared/game_state.h>
#include <shared/message_types.h>
#include <typeinfo>
#include "gmock/gmock.h"
#include "gtest/gtest.h"

using ::testing::_;
using ::testing::InSequence;
using ::testing::Truly;

TEST(ServerLibraryTest, CreateLobby)
{
    auto is_create_lobby_response_message = [](shared::ServerToClientMessage *message)
    {
        const shared::CreateLobbyResponseMessage *create_lobby_response_msg =
                dynamic_cast<shared::CreateLobbyResponseMessage *>(message);
        return create_lobby_response_msg != nullptr;
    };

    auto is_failure_message = [](shared::ServerToClientMessage *message)
    {
        const shared::ResultResponseMessage *result_msg = dynamic_cast<shared::ResultResponseMessage *>(message);
        return result_msg && !result_msg->success;
    };

    server::MockMessageInterface *message_interface = new server::MockMessageInterface();
    server::LobbyManager lobby_manager(*message_interface);
    shared::PlayerBase::id_t player_1 = "Max";
    shared::PlayerBase::id_t player_2 = "Peter";

    shared::CreateLobbyRequestMessage request1;
    request1.game_id = "123";
    request1.message_id = "456";
    request1.player_id = player_1;

    shared::CreateLobbyRequestMessage request2;
    request2.game_id = "123";
    request2.message_id = "789";
    request2.player_id = player_2;

    // All expected function calls of send_message
    EXPECT_CALL(*message_interface, send_message(Truly(is_failure_message), _))
            .Times(1); // Error for second time creating lobby
    EXPECT_CALL(*message_interface, send_message(Truly(is_create_lobby_response_message), _))
            .Times(1); // Success for first time creating lobby

    auto games = lobby_manager.get_games();
    ASSERT_EQ(games->empty(), true) << "LobbyManager should be empty at the beginning";

    lobby_manager.create_lobby(request1);

    ASSERT_EQ(games->size(), 1) << "LobbyManager should contain one lobby after creating one";
    ASSERT_EQ(games->find("123") != games->end(), true) << "Lobby with id 123 should exist";
    ASSERT_EQ(games->at("123").get_game_master(), player_1) << "Game master should be player_1";
    ASSERT_EQ(games->at("123").get_full_game_state().get_players().size(), 1)
            << "There should be one player in the lobby";

    // No new lobby should be created, because game with id 123 already exists
    lobby_manager.create_lobby(request2);
    ASSERT_EQ(games->size(), 1);
    // Check if the lobby with id really 123 exists
    ASSERT_EQ(games->find("123") != games->end(), true);
    // Check if the game_master didn't change
    ASSERT_EQ(games->at("123").get_game_master(), player_1);
    ASSERT_EQ(games->at("123").get_full_game_state().get_players().size(), 1);

    delete message_interface;
}

TEST(ServerLibraryTest, JoinLobby)
{
    auto is_success_message = [](shared::ServerToClientMessage *message)
    {
        const shared::ResultResponseMessage *result_msg = dynamic_cast<shared::ResultResponseMessage *>(message);
        return result_msg && result_msg->success;
    };

    auto is_failure_message = [](shared::ServerToClientMessage *message)
    {
        const shared::ResultResponseMessage *result_msg = dynamic_cast<shared::ResultResponseMessage *>(message);
        return result_msg && !result_msg->success;
    };

    auto is_join_lobby_broadcast_message = [](shared::ServerToClientMessage *message)
    {
        const shared::JoinLobbyBroadcastMessage *join_lobby_broadcast_msg =
                dynamic_cast<shared::JoinLobbyBroadcastMessage *>(message);
        return join_lobby_broadcast_msg != nullptr;
    };

    server::MockMessageInterface *message_interface = new server::MockMessageInterface();
    server::LobbyManager lobby_manager(*message_interface);
    shared::PlayerBase::id_t player_1 = "Max";
    shared::PlayerBase::id_t player_2 = "Peter";
    shared::PlayerBase::id_t player_3 = "Paul";
    shared::PlayerBase::id_t player_4 = "John";
    shared::PlayerBase::id_t player_5 = "George";

    shared::CreateLobbyRequestMessage request1;
    request1.game_id = "123";
    request1.message_id = "456";
    request1.player_id = player_1;

    shared::JoinLobbyRequestMessage request2;
    request2.game_id = "123";
    request2.message_id = "789";
    request2.player_id = player_2;

    shared::JoinLobbyRequestMessage false_request3;
    false_request3.game_id = "222";
    false_request3.message_id = "101";
    false_request3.player_id = player_3;

    shared::JoinLobbyRequestMessage corrected_request3;
    corrected_request3.game_id = "123";
    corrected_request3.message_id = "102";
    corrected_request3.player_id = player_3;

    shared::JoinLobbyRequestMessage request4;
    request4.game_id = "123";
    request4.message_id = "103";
    request4.player_id = player_4;

    shared::JoinLobbyRequestMessage request5;
    request5.game_id = "123";
    request5.message_id = "104";
    request5.player_id = player_5;

    auto games = lobby_manager.get_games();

    lobby_manager.create_lobby(request1);

    // All expected function calls of send_message
    {
        InSequence s;
        EXPECT_CALL(*message_interface, send_message(Truly(is_join_lobby_broadcast_message), player_1)).Times(1);
        EXPECT_CALL(*message_interface, send_message(Truly(is_success_message), player_2)).Times(1);
        EXPECT_CALL(*message_interface, send_message(Truly(is_failure_message), player_2)).Times(1);
        EXPECT_CALL(*message_interface, send_message(Truly(is_failure_message), player_3)).Times(1);
        // 1x ResultResponseMessage + 2x JoinLobbyBroadcastMessage 
        // + 1x ResultResponseMessage + 3x JoinLobbyBroadcastMessage
        EXPECT_CALL(*message_interface, send_message(_, _)).Times(7); 
        EXPECT_CALL(*message_interface, send_message(Truly(is_failure_message), player_5)).Times(1);
    }
    lobby_manager.join_lobby(request2);
    ASSERT_EQ(games->at("123").get_full_game_state().get_players().size(), 2)
            << "There should be two players in the lobby";
    ASSERT_EQ(games->at("123").get_full_game_state().get_players().at(1).getId(), player_2)
            << "Player 2 should be in the lobby";

    // Player 2 should not be added again
    lobby_manager.join_lobby(request2);
    ASSERT_EQ(games->at("123").get_full_game_state().get_players().size(), 2)
            << "There should still be two players in the lobby";

    // Player 3 should not be able to join the lobby with id 123
    lobby_manager.join_lobby(false_request3);
    ASSERT_EQ(games->at("123").get_full_game_state().get_players().size(), 2)
            << "There should still be two players in the lobby";

    // Player 3 should be able to join the lobby with id 123
    lobby_manager.join_lobby(corrected_request3);

    // Player 4 should be able to join the lobby
    lobby_manager.join_lobby(request4);

    // Player 5 should not be able to join because the lobby is full
    lobby_manager.join_lobby(request5);
    ASSERT_EQ(games->at("123").get_full_game_state().get_players().size(), 4)
            << "There should still be four players in the lobby";
    delete message_interface;
}

TEST(ServerLibraryTest, StartGame)
{
    
}