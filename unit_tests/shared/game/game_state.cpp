// reduced_game_state_test.cpp

#include <gtest/gtest.h>
#include <shared/game/game_state/reduced_game_state.h>

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
    EXPECT_EQ(game_state.reduced_player.getId(), "player");

    // Check enemies
    ASSERT_EQ(game_state.enemies.size(), 2);
    EXPECT_EQ(game_state.enemies[0].getId(), "enemy1");
    EXPECT_EQ(game_state.enemies[1].getId(), "enemy2");
}
