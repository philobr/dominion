#include <server/lobbies/lobby.h>
#include <server/lobbies/lobby_manager.h>
#include <shared/message_types.h>
#include <shared/utils/logger.h>
#include <shared/utils/test_helpers.h>
#include <typeinfo>
#include "mock_templates.h"

/**
 * @brief This casts the given message to a ClientToServerMessage and calls the lobby manager
 */
#define LOBBY_MANAGER_CALL(msg)                                                                                        \
    auto casted_##msg = std::unique_ptr<shared::ClientToServerMessage>((msg).release());                               \
    lobby_manager.handleMessage(casted_##msg)

TEST(ServerLibraryTest, CreateLobby)
{
    std::shared_ptr<MockMessageInterface> message_interface = std::make_shared<MockMessageInterface>();
    server::LobbyManager lobby_manager(message_interface);
    shared::PlayerBase::id_t player_1 = "Max";
    shared::PlayerBase::id_t player_2 = "Peter";

    auto create_lobby = std::make_unique<shared::CreateLobbyRequestMessage>("123", player_1);
    auto create_lobby_again = std::make_unique<shared::CreateLobbyRequestMessage>("123", player_2);

    // All expected function calls of sendMessage
    {
        InSequence s;
        EXPECT_CALL(*message_interface, sendMessage(IsCreateLobbyResponseMessage(), _))
                .Times(1); // Success for first time creating lobby
        EXPECT_CALL(*message_interface, sendMessage(IsFailureMessage(), _))
                .Times(1); // Error for second time creating lobby
    }

    const std::map<std::string, std::shared_ptr<server::Lobby>> &games = lobby_manager.getGames();
    ASSERT_EQ(games.empty(), true) << "LobbyManager should be empty at the beginning";

    LOBBY_MANAGER_CALL(create_lobby);

    ASSERT_EQ(games.size(), 1) << "LobbyManager should contain one lobby after creating one";
    ASSERT_EQ(games.find("123") != games.end(), true) << "Lobby with id 123 should exist";
    ASSERT_EQ(games.at("123")->getGameMaster(), player_1) << "Game master should be player_1";
    ASSERT_EQ(games.at("123")->getPlayers().size(), 1) << "There should be one player in the lobby";

    // No new lobby should be created, because game with id 123 already exists
    LOBBY_MANAGER_CALL(create_lobby_again);
    ASSERT_EQ(games.size(), 1);
    // Check if the lobby with id really 123 exists
    ASSERT_EQ(games.find("123") != games.end(), true);
    // Check if the game_master didn't change
    ASSERT_EQ(games.at("123")->getGameMaster(), player_1);
    ASSERT_EQ(games.at("123")->getPlayers().size(), 1);
}

TEST(ServerLibraryTest, JoinLobby)
{
    std::shared_ptr<MockMessageInterface> message_interface = std::make_shared<MockMessageInterface>();
    server::LobbyManager lobby_manager(message_interface);
    shared::PlayerBase::id_t player_1 = "Max";
    shared::PlayerBase::id_t player_2 = "Peter";
    shared::PlayerBase::id_t player_3 = "Paul";
    shared::PlayerBase::id_t player_4 = "John";
    shared::PlayerBase::id_t player_5 = "George";

    auto create_lobby = std::make_unique<shared::CreateLobbyRequestMessage>("123", player_1);
    auto join_lobby_1 = std::make_unique<shared::JoinLobbyRequestMessage>("123", player_2);
    auto join_lobby_1_invalid = std::make_unique<shared::JoinLobbyRequestMessage>("123", player_2);
    auto join_nonexistent_lobby = std::make_unique<shared::JoinLobbyRequestMessage>("222", player_3);
    auto join_lobby_2 = std::make_unique<shared::JoinLobbyRequestMessage>("123", player_3);
    auto join_lobby_3 = std::make_unique<shared::JoinLobbyRequestMessage>("123", player_4);
    auto join_lobby_4 = std::make_unique<shared::JoinLobbyRequestMessage>("123", player_5);

    const std::map<std::string, std::shared_ptr<server::Lobby>> &games = lobby_manager.getGames();

    LOBBY_MANAGER_CALL(create_lobby);

    // All expected function calls of sendMessage
    {
        InSequence s;
        // request 2
        EXPECT_CALL(*message_interface, sendMessage(IsSuccessMessage(), player_2)).Times(1);
        EXPECT_CALL(*message_interface, sendMessage(IsJoinLobbyBroadcastMessage(), _)).Times(2);
        // request 2 again
        EXPECT_CALL(*message_interface, sendMessage(IsFailureMessage(), player_2)).Times(1);
        // false_request 3
        EXPECT_CALL(*message_interface, sendMessage(IsFailureMessage(), player_3)).Times(1);
        // corrected_request 3 (1x ResultResponseMessage + 3x JoinLobbyBroadcastMessage)
        // + request 4 (1x ResultResponseMessage + 4x JoinLobbyBroadcastMessage)
        EXPECT_CALL(*message_interface, sendMessage(_, _)).Times(9);
        // request 5
        EXPECT_CALL(*message_interface, sendMessage(IsFailureMessage(), player_5)).Times(1);
    }

    LOBBY_MANAGER_CALL(join_lobby_1);
    ASSERT_EQ(games.at("123")->getPlayers().size(), 2) << "There should be two players in the lobby";
    ASSERT_EQ(games.at("123")->getPlayers().at(1), player_2) << "Player 2 should be in the lobby";

    // Player 2 should not be added again
    LOBBY_MANAGER_CALL(join_lobby_1_invalid);
    ASSERT_EQ(games.at("123")->getPlayers().size(), 2) << "There should still be two players in the lobby";

    // Player 3 should not be able to join the lobby with id 123
    LOBBY_MANAGER_CALL(join_nonexistent_lobby);
    ASSERT_EQ(games.at("123")->getPlayers().size(), 2) << "There should still be two players in the lobby";

    // Player 3 should be able to join the lobby with id 123
    LOBBY_MANAGER_CALL(join_lobby_2);

    // Player 4 should be able to join the lobby
    LOBBY_MANAGER_CALL(join_lobby_3);

    // Player 5 should not be able to join because the lobby is full
    LOBBY_MANAGER_CALL(join_lobby_4);
    ASSERT_EQ(games.at("123")->getPlayers().size(), 4) << "There should still be four players in the lobby";
}

TEST(ServerLibraryTest, StartGame)
{
    std::shared_ptr<MockMessageInterface> message_interface = std::make_shared<MockMessageInterface>();
    server::LobbyManager lobby_manager(message_interface);
    shared::PlayerBase::id_t game_master = "Max";
    shared::PlayerBase::id_t not_game_master = "Peter";

    const std::string game_id = "123";
    const std::string invalid_game_id = "abc";

    auto create_lobby = std::make_unique<shared::CreateLobbyRequestMessage>(game_id, game_master);
    auto join_lobby = std::make_unique<shared::JoinLobbyRequestMessage>(game_id, not_game_master);

    std::vector<shared::CardBase::id_t> selected_cards = getValidKingdomCards();

    // Start game request with wrong game_id
    auto invalid_game_id_message =
            std::make_unique<shared::StartGameRequestMessage>(invalid_game_id, game_master, selected_cards);

    // Start game request not as game_master
    auto invalid_game_master =
            std::make_unique<shared::StartGameRequestMessage>(game_id, not_game_master, selected_cards);

    // Start game request as game_master
    auto valid_request = std::make_unique<shared::StartGameRequestMessage>(game_id, game_master, selected_cards);

    LOG(DEBUG) << "StartGameRequestMessage(player, game, message): " << valid_request->player_id << " "
               << valid_request->game_id << " " << valid_request->message_id;

    LOBBY_MANAGER_CALL(create_lobby);
    LOBBY_MANAGER_CALL(join_lobby);

    // All expected function calls of sendMessage
    {
        InSequence s;
        // start_game_invalid, invalid game_id
        EXPECT_CALL(*message_interface, sendMessage(IsFailureMessage(), game_master)).Times(1);
        // start_game_invalid_again, not game master
        EXPECT_CALL(*message_interface, sendMessage(IsFailureMessage(), not_game_master)).Times(1);
        // start_game_valid, correct game_id and game_master
        EXPECT_CALL(*message_interface, sendMessage(_, _)).Times(4); // num players * 3
    }

    LOBBY_MANAGER_CALL(invalid_game_id_message);
    LOBBY_MANAGER_CALL(invalid_game_master);
    LOBBY_MANAGER_CALL(valid_request);
}

TEST(ServerLibraryTest, ReceiveAction)
{
    std::shared_ptr<MockMessageInterface> message_interface = std::make_shared<MockMessageInterface>();
    server::LobbyManager lobby_manager(message_interface);
    shared::PlayerBase::id_t player_1 = "Max";
    shared::PlayerBase::id_t player_2 = "Peter";
    shared::PlayerBase::id_t player_3 = "Paul";

    // Setup
    auto create_lobby = std::make_unique<shared::CreateLobbyRequestMessage>("123", player_1);
    auto join_lobby = std::make_unique<shared::JoinLobbyRequestMessage>("123", player_2);

    LOBBY_MANAGER_CALL(create_lobby);
    LOBBY_MANAGER_CALL(join_lobby);
    // Finish setup

    // ActionDecision for a lobby that doesn't exist
    auto nonexistent_lobby_action = std::make_unique<shared::ActionDecisionMessage>(
            "456", player_1, std::make_unique<shared::PlayActionCardDecision>("Village"));

    // ActionDecision for a player that is not in the lobby
    auto player_not_in_lobby = std::make_unique<shared::ActionDecisionMessage>(
            "123", player_3, std::make_unique<shared::BuyCardDecision>("Copper"));

    // ActionDecision for a game that hasn't started yet
    auto unstarted_game_action = std::make_unique<shared::ActionDecisionMessage>(
            "123", player_1, std::make_unique<shared::PlayActionCardDecision>("Village"));

    // First part of expected function calls of sendMessage
    {
        InSequence s;
        // nonexistent_lobby_action
        EXPECT_CALL(*message_interface, sendMessage(IsFailureMessage(), player_1)).Times(1);
        // player_not_in_lobby
        EXPECT_CALL(*message_interface, sendMessage(IsFailureMessage(), player_3)).Times(1);
        // unstarted_game_action
        EXPECT_CALL(*message_interface, sendMessage(IsFailureMessage(), player_1)).Times(1);
        EXPECT_CALL(*message_interface, sendMessage(IsSuccessMessage(), player_1)).Times(1);
        EXPECT_CALL(*message_interface, sendMessage(IsSuccessMessage(), player_2)).Times(1);
    }

    LOBBY_MANAGER_CALL(nonexistent_lobby_action);
    LOBBY_MANAGER_CALL(player_not_in_lobby);
    LOBBY_MANAGER_CALL(unstarted_game_action);
}
#undef LOBBY_MANAGER_CALL