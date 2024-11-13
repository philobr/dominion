// reduced_game_state_test.cpp

#include <gtest/gtest.h>
#include <shared/game/game_state/reduced_game_state.h>

// Begin test cases

TEST(ReducedGameStateTest, DefaultConstructor)
{
    shared::ReducedGameState game_state;

    // Check that members are default-initialized
    // Assuming default constructor of Board, ReducedPlayer, and vectors initialize as expected
    // You may need to adjust this based on actual default constructors

    // Since we cannot compare complex objects directly without appropriate operators,
    // we can check if the enemies vector is empty and active_player is empty string
    EXPECT_EQ(game_state.enemies.size(), 0);
    EXPECT_EQ(game_state.active_player, "");
}

TEST(ReducedGameStateTest, ParameterizedConstructor)
{
    // Create a Board object
    shared::Board board;

    shared::PlayerBase player("player"), enemy1("enemy1"), enemy2("enemy2");

    // Create a list of ReducedEnemies
    std::vector<shared::ReducedEnemy> enemies = {shared::ReducedEnemy(enemy1, 0), shared::ReducedEnemy(enemy2, 0)};
    // Set the active player ID
    shared::PlayerBase::id_t active_player = player.getId();
    shared::ReducedPlayer reduced_player(player, {});
    // Create the ReducedGameState
    shared::ReducedGameState game_state(board, reduced_player, enemies, active_player);

    // Check that the members are initialized correctly
    EXPECT_EQ(game_state.active_player, active_player);

    // Since Board and ReducedPlayer are complex objects, we would need to have appropriate comparison operators
    // For now, we can check IDs or sizes where applicable

    // Check player ID
    EXPECT_EQ(game_state.player.getId(), "player");

    // Check enemies
    ASSERT_EQ(game_state.enemies.size(), 2);
    EXPECT_EQ(game_state.enemies[0].getId(), "enemy1");
    EXPECT_EQ(game_state.enemies[1].getId(), "enemy2");
}

TEST(ReducedGameStateTest, ModifyEnemies)
{
    // Create a ReducedGameState with initial enemies
    shared::ReducedGameState game_state;

    shared::PlayerBase enemy1("enemy1"), enemy2("enemy2");
    game_state.enemies.push_back(shared::ReducedEnemy(enemy1, 0));
    game_state.enemies.push_back(shared::ReducedEnemy(enemy2, 0));

    // Check initial size
    ASSERT_EQ(game_state.enemies.size(), 2);

    // Add another enemy
    shared::PlayerBase enemy3("enemy3");
    game_state.enemies.push_back(shared::ReducedEnemy(enemy3, 0));

    // Check new size
    EXPECT_EQ(game_state.enemies.size(), 3);

    // Remove an enemy
    game_state.enemies.pop_back();

    // Check size again
    EXPECT_EQ(game_state.enemies.size(), 2);
}

TEST(ReducedGameStateTest, ActivePlayer)
{
    // Create a ReducedGameState and set active player
    shared::ReducedGameState game_state;
    game_state.active_player = "player1";

    // Check active player
    EXPECT_EQ(game_state.active_player, "player1");

    // Change active player
    game_state.active_player = "player2";

    // Check active player again
    EXPECT_EQ(game_state.active_player, "player2");
}

TEST(ReducedGameStateTest, BoardState)
{
    // Create a Board object with some state
    shared::Board board;
    // For testing purposes, let's assume Board has an isGameOver() method

    // Create a ReducedGameState with this board
    shared::ReducedGameState game_state;
    game_state.board = board;

    // Check if the game is over
    EXPECT_FALSE(game_state.board.isGameOver());
}
