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
        EXPECT_CALL(*message_interface, sendMessage(IsJoinLobbyBroadcastMessage(), _)).Times(2);
        EXPECT_CALL(*message_interface, sendMessage(IsSuccessMessage(), player_2)).Times(1);
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
    shared::PlayerBase::id_t player_1 = "Max";
    shared::PlayerBase::id_t player_2 = "Peter";

    auto create_lobby = std::make_unique<shared::CreateLobbyRequestMessage>("123", player_1);
    auto join_lobby = std::make_unique<shared::JoinLobbyRequestMessage>("123", player_2);

    std::vector<shared::CardBase::id_t> selected_cards = getValidKingdomCards();

    // Start game request with wrong game_id
    auto start_game_invalid = std::make_unique<shared::StartGameRequestMessage>("abc", player_1, selected_cards);

    // Start game request not as game_master
    auto start_game_valid = std::make_unique<shared::StartGameRequestMessage>("123", player_2, selected_cards);

    // Start game request as game_master
    auto start_game_again_invalid = std::make_unique<shared::StartGameRequestMessage>("123", player_1, selected_cards);

    LOG(DEBUG) << "StartGameRequestMessage(player, game, message): " << start_game_invalid->player_id << " "
               << start_game_invalid->game_id << " " << start_game_invalid->message_id;

    LOBBY_MANAGER_CALL(create_lobby);
    LOBBY_MANAGER_CALL(join_lobby);

    // All expected function calls of sendMessage
    {
        InSequence s;
        // request3
        EXPECT_CALL(*message_interface, sendMessage(IsFailureMessage(), player_1)).Times(1);
        // request4
        EXPECT_CALL(*message_interface, sendMessage(IsFailureMessage(), player_2)).Times(1);
        // request5
        EXPECT_CALL(*message_interface, sendMessage(_, _)).Times(4);
    }

    LOBBY_MANAGER_CALL(start_game_invalid);
    LOBBY_MANAGER_CALL(start_game_valid);
    LOBBY_MANAGER_CALL(start_game_again_invalid);
    // TODO: Check if the game started correctly
}


// TODO: those tests fail and i dont quite understand what they are supposed to do
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

    std::vector<shared::CardBase::id_t> selected_cards = getValidKingdomCards();
    auto start_lobby = std::make_unique<shared::StartGameRequestMessage>("123", player_1, selected_cards);

    LOBBY_MANAGER_CALL(create_lobby);
    LOBBY_MANAGER_CALL(join_lobby);
    // Finish setup

    // ActionDecision for a lobby that doesn't exist
    auto nonexistent_lobby_action = std::make_unique<shared::ActionDecisionMessage>(
            "456", player_1, std::make_unique<shared::PlayActionCardDecision>(1));

    // ActionDecision for a game that hasn't started yet
    auto unstarted_game_action = std::make_unique<shared::ActionDecisionMessage>(
            "123", player_1, std::make_unique<shared::PlayActionCardDecision>(1));

    // ActionDecision for a player that is not in the lobby
    auto player_not_in_lobby = std::make_unique<shared::ActionDecisionMessage>(
            "123", player_3, std::make_unique<shared::BuyCardDecision>("Village"));

    // ActionDecision should be handled correctly
    auto almost_correct_action = std::make_unique<shared::ActionDecisionMessage>(
            "123", player_1, std::make_unique<shared::BuyCardDecision>("Village"));

    // First part of expected function calls of sendMessage
    {
        InSequence s;
        // request4
        EXPECT_CALL(*message_interface, sendMessage(IsFailureMessage(), player_1)).Times(1);
        // request5
        EXPECT_CALL(*message_interface, sendMessage(IsFailureMessage(), player_1)).Times(1);
        // Start game messages
        EXPECT_CALL(*message_interface, sendMessage(_, _)).Times(4);
        // request6
        EXPECT_CALL(*message_interface, sendMessage(IsFailureMessage(), player_3)).Times(1);
        // request7
        EXPECT_CALL(*message_interface, sendMessage(_, player_1)).Times(1);
    }

    LOBBY_MANAGER_CALL(nonexistent_lobby_action);
    auto casted_request5 = std::unique_ptr<shared::ClientToServerMessage>(unstarted_game_action.release());
    EXPECT_THROW(lobby_manager.handleMessage(casted_request5), std::runtime_error);

    LOBBY_MANAGER_CALL(start_lobby);
    LOBBY_MANAGER_CALL(player_not_in_lobby);
    LOBBY_MANAGER_CALL(
            almost_correct_action); // TODO: THIS WILL FAIL SOME DAY, CHECK IN GAME_INTERFACE::HANDLE_BUY_DECISION!
}

#undef LOBBY_MANAGER_CALL