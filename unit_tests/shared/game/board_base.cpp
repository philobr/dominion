#include <gtest/gtest.h>
#include <shared/game/game_state/board_base.h>

TEST(BoardTest, InitialiseTreasureCards)
{
    shared::Board board;
    size_t player_count = 2;

    board.initialise_treasure_cards(player_count);

    ASSERT_EQ(board.treasure_cards.size(), 3); // Copper, Silver, Gold

    // Copper count should be 60 - (7 * player_count)
    size_t expected_copper_count = 60 - (7 * player_count);
    EXPECT_EQ(board.treasure_cards[0].card, "Copper");
    EXPECT_EQ(board.treasure_cards[0].count, expected_copper_count);

    EXPECT_EQ(board.treasure_cards[1].card, "Silver");
    EXPECT_EQ(board.treasure_cards[1].count, 40);

    EXPECT_EQ(board.treasure_cards[2].card, "Gold");
    EXPECT_EQ(board.treasure_cards[2].count, 30);
}

TEST(BoardTest, InitialiseVictoryCardsTwoPlayers)
{
    shared::Board board;
    size_t player_count = 2;

    board.initialise_victory_cards(player_count);

    ASSERT_EQ(board.victory_cards.size(), 4); // Estate, Duchy, Province (+curse)

    size_t expected_count = 8; // For two players

    EXPECT_EQ(board.victory_cards[0].card, "Estate");
    EXPECT_EQ(board.victory_cards[0].count, expected_count);

    EXPECT_EQ(board.victory_cards[1].card, "Duchy");
    EXPECT_EQ(board.victory_cards[1].count, expected_count);

    EXPECT_EQ(board.victory_cards[2].card, "Province");
    EXPECT_EQ(board.victory_cards[2].count, expected_count);
}

TEST(BoardTest, InitialiseVictoryCardsThreePlayers)
{
    shared::Board board;
    size_t player_count = 3;

    board.initialise_victory_cards(player_count);

    ASSERT_EQ(board.victory_cards.size(), 4); // Estate, Duchy, Province (+curse)

    size_t expected_count = 12; // For three or more players

    EXPECT_EQ(board.victory_cards[0].card, "Estate");
    EXPECT_EQ(board.victory_cards[0].count, expected_count);

    EXPECT_EQ(board.victory_cards[1].card, "Duchy");
    EXPECT_EQ(board.victory_cards[1].count, expected_count);

    EXPECT_EQ(board.victory_cards[2].card, "Province");
    EXPECT_EQ(board.victory_cards[2].count, expected_count);
}

TEST(BoardTest, CountEmptyPiles)
{
    shared::Board board;

    // Add some piles with counts
    board.kingdom_cards = {
            {"Smithy", 10}, {"Village", 0}, {"Market", 10}, {"Mine", 0}, {"Moat", 0},
    };

    board.victory_cards = {
            {"Estate", 5},
            {"Duchy", 0},
            {"Province", 3},
    };

    board.treasure_cards = {
            {"Copper", 0},
            {"Silver", 40},
            {"Gold", 30},
    };

    size_t empty_piles = board.countEmptyPiles();

    // Empty piles: Village, Mine, Moat, Duchy, Copper => total 5 empty piles
    EXPECT_EQ(empty_piles, 5);
}

TEST(BoardTest, IsGameOverProvinceEmpty)
{
    shared::Board board;
    board.initialise_victory_cards(2);

    // Set Province count to 0
    auto it = std::find_if(board.victory_cards.begin(), board.victory_cards.end(),
                           [](const shared::Pile &pile) { return pile.card == "Province"; });

    if ( it != board.victory_cards.end() ) {
        it->count = 0;
    }

    EXPECT_TRUE(board.isGameOver());
}

TEST(BoardTest, IsGameOverThreeEmptyPiles)
{
    shared::Board board;

    // Initialize some piles
    board.kingdom_cards = {
            {"Smithy", 0},
            {"Village", 0},
            {"Market", 0},
            {"Mine", 10},
    };

    board.victory_cards = {
            {"Estate", 5},
            {"Duchy", 5},
            {"Province", 5},
    };

    board.treasure_cards = {
            {"Copper", 46},
            {"Silver", 40},
            {"Gold", 30},
    };

    // Since three kingdom card piles are empty, the game should be over
    EXPECT_TRUE(board.isGameOver());
}

TEST(BoardTest, IsGameOverNotOver)
{
    shared::Board board;

    // Initialize some piles
    board.kingdom_cards = {
            {"Smithy", 10},
            {"Village", 10},
            {"Market", 10},
            {"Mine", 10},
    };

    board.victory_cards = {
            {"Estate", 5},
            {"Duchy", 5},
            {"Province", 5},
    };

    board.treasure_cards = {
            {"Copper", 46},
            {"Silver", 40},
            {"Gold", 30},
    };

    // No empty province pile and less than three empty piles
    EXPECT_FALSE(board.isGameOver());
}
