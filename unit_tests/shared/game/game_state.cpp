// reduced_game_state_test.cpp

#include <gtest/gtest.h>
#include <shared/game/game_state/reduced_game_state.h>
#include <shared/utils/test_helpers.h>

TEST(ReducedGameStateTest, Json2WayConversion)
{
    // Create a list of ReducedEnemies
    shared::PlayerBase enemy1("Charlie");
    shared::PlayerBase enemy2("Jacob");
    std::vector<reduced::Enemy::ptr_t> enemies;
    enemies.emplace_back(reduced::Enemy::make(enemy1, 5));
    enemies.emplace_back(reduced::Enemy::make(enemy2, 7));

    // Set the active player
    shared::PlayerBase::id_t active_player = "Charlie";

    // Create the player
    shared::PlayerBase player("Alice");
    reduced::Player::ptr_t reduced_player = reduced::Player::make(player, {});

    // Create the board
    auto kingdom_cards = get_valid_kingdom_cards();
    shared::Board::ptr_t board = shared::Board::make(kingdom_cards, 3);

    // Create the ReducedGameState
    reduced::GameState expected(board, std::move(reduced_player), std::move(enemies), active_player);

    auto json = expected.toJson();

    // Convert back to ReducedGameState
    std::unique_ptr<reduced::GameState> actual = reduced::GameState::fromJson(json);

    ASSERT_NE(actual, nullptr);
    EXPECT_EQ(*actual, expected);
}

TEST(ReducedGameStateTest, ParameterizedConstructor)
{
    // Create a list of ReducedEnemies
    shared::PlayerBase enemy1("enemy1");
    shared::PlayerBase enemy2("enemy2");
    std::vector<reduced::Enemy::ptr_t> enemies;
    enemies.emplace_back(reduced::Enemy::make(enemy1, 0));
    enemies.emplace_back(reduced::Enemy::make(enemy2, 0));

    // Set the active player ID
    shared::PlayerBase::id_t active_player = "player";
    shared::PlayerBase player(active_player);
    reduced::Player::ptr_t reduced_player = reduced::Player::make(player, {});

    // Create a Board object
    auto kingdom_cards = get_valid_kingdom_cards();
    shared::Board::ptr_t board = shared::Board::make(kingdom_cards, 2);

    // Create the ReducedGameState
    reduced::GameState game_state(board, std::move(reduced_player), std::move(enemies), active_player);

    // Check that the members are initialized correctly
    EXPECT_EQ(game_state.active_player, active_player);

    // Since Board and ReducedPlayer are complex objects, we would need to have appropriate comparison operators
    // For now, we can check IDs or sizes where applicable

    // Check player ID
    EXPECT_EQ(game_state.reduced_player->getId(), "player");

    // Check enemies
    ASSERT_EQ(game_state.reduced_enemies.size(), 2);
    EXPECT_EQ(game_state.reduced_enemies[0]->getId(), "enemy1");
    EXPECT_EQ(game_state.reduced_enemies[1]->getId(), "enemy2");
}
