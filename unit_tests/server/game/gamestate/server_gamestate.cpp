// game_state_test.cpp

#include <gtest/gtest.h>
#include <memory>
#include <string>
#include <vector>

#include <server/game/game_state/game_state.h>


TEST(GameStateTest, DefaultConstructor)
{
    // Create a GameState instance using the default constructor
    server::GameState game_state;

    // Since the default constructor doesn't initialize players or board, we expect certain values
    // We might not be able to test much here unless default behavior is defined
}

TEST(GameStateTest, ParameterizedConstructor)
{
    // Initialize test data
    std::vector<shared::CardBase::id_t> selected_cards = {"Smithy",  "Village",  "Market", "Mine",   "Moat",
                                                          "Militia", "Workshop", "Cellar", "Chapel", "Woodcutter"};

    std::vector<server::Player::id_t> player_ids = {"player1", "player2", "player3"};

    // Create a GameState instance with the provided cards and players
    server::GameState game_state(selected_cards, player_ids);

    // Test that players are initialized
    for ( const auto &id : player_ids ) {
        EXPECT_NO_THROW(game_state.get_player(id));
    }

    // Test that the board is initialized
    // We may need to access the board through game_state
    // Since board is private, we might not be able to access it directly
    // But we can infer its existence through the behavior of other methods
}

TEST(GameStateTest, GetCurrentPlayerId)
{
    std::vector<shared::CardBase::id_t> selected_cards = {"Village", "Market", "card3", "card4", "card5",
                                                          "card6",   "card7",  "card8", "card9", "card10"};
    std::vector<server::Player::id_t> player_ids = {"player1", "player2"};

    server::GameState game_state(selected_cards, player_ids);

    // Initially, the current player should be the first in the player_order
    EXPECT_EQ(game_state.getCurrentPlayerId(), "player1");
}

TEST(GameStateTest, GetCurrentPlayer)
{
    std::vector<shared::CardBase::id_t> selected_cards = {"Village", "Market", "card3", "card4", "card5",
                                                          "card6",   "card7",  "card8", "card9", "card10"};
    std::vector<server::Player::id_t> player_ids = {"player1", "player2"};

    server::GameState game_state(selected_cards, player_ids);

    // Get the current player
    server::Player current_player = game_state.get_current_player();

    // Check that the current player's ID matches
    EXPECT_EQ(current_player.getId(), "player1");
}

TEST(GameStateTest, GetPlayerById)
{
    std::vector<shared::CardBase::id_t> selected_cards = {"Village", "Market", "card3", "card4", "card5",
                                                          "card6",   "card7",  "card8", "card9", "card10"};
    std::vector<server::Player::id_t> player_ids = {"player1", "player2", "player3"};

    server::GameState game_state(selected_cards, player_ids);

    // Test getting each player by ID
    for ( const auto &id : player_ids ) {
        server::Player &player = game_state.get_player(id);
        EXPECT_EQ(player.getId(), id);
    }

    // Test that requesting a non-existent player throws an exception
    EXPECT_THROW(game_state.get_player("nonexistent_player"), std::out_of_range);
}

TEST(GameStateTest, StartTurn)
{
    // Since start_turn is a private method, we can't call it directly
    // However, we can test the effects of starting a turn by initializing a game and checking the state

    std::vector<shared::CardBase::id_t> selected_cards = {"Village", "Market", "card3", "card4", "card5",
                                                          "card6",   "card7",  "card8", "card9", "card10"};
    std::vector<server::Player::id_t> player_ids = {"player1", "player2"};

    server::GameState game_state(selected_cards, player_ids);

    // We can simulate starting the game, which should call start_turn internally
    // Assume that start_game is public or called in the constructor (adjust as necessary)

    // Check that the current player has the correct initial state
    server::Player &current_player = game_state.get_current_player();
    EXPECT_EQ(current_player.getActions(), 1);
    EXPECT_EQ(current_player.getBuys(), 1);
    EXPECT_EQ(current_player.getTreasure(), 0);

    // Check that the player has drawn their initial hand (usually 5 cards)
    // This depends on how the draw method is implemented
    // For testing purposes, we can check if hand_cards is not empty
    EXPECT_EQ(current_player.get_hand_cards().size(), 5);
}
