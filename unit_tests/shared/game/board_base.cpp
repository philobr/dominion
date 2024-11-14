#include <algorithm>
#include <gtest/gtest.h>

// Include necessary headers for shared::Board, shared::CardBase, etc.
#include <shared/game/cards/card_base.h> // Adjust the path according to your project structure
#include <shared/game/game_state/board_base.h>
#include <shared/utils/test_helpers.h>

// Define the TestableSharedBoard class to expose protected members and methods
class TestableSharedBoard : public shared::Board
{
public:
    TestableSharedBoard(const std::vector<shared::CardBase::id_t> &kingdom_cards, size_t player_count) :
        Board(kingdom_cards, player_count)
    {}

    // Expose protected methods as public for testing
    using shared::Board::countEmptyPiles;
    using shared::Board::initialise_treasure_cards;
    using shared::Board::initialise_victory_cards;
    using shared::Board::isGameOver;

    // Expose protected member variables for testing access
    std::vector<shared::Pile> &getTreasureCards() { return treasure_cards; }
    std::vector<shared::Pile> &getVictoryCards() { return victory_cards; }
    std::vector<shared::Pile> &getKingdomCards() { return kingdom_cards; }
};

// Test for initializing victory cards with three players
TEST(BoardTest, InitialiseVictoryCardsThreePlayers)
{
    // Setup
    auto kingdom_cards = get_valid_kingdom_cards();

    size_t player_count = 3;
    TestableSharedBoard board(kingdom_cards, player_count);

    board.initialise_victory_cards(player_count);

    ASSERT_EQ(board.getVictoryCards().size(), 4); // Estate, Duchy, Province, Curse

    size_t expected_count = 12; // For three or more players

    EXPECT_EQ(board.getVictoryCards()[0].card, "Estate");
    EXPECT_EQ(board.getVictoryCards()[0].count, expected_count);

    EXPECT_EQ(board.getVictoryCards()[1].card, "Duchy");
    EXPECT_EQ(board.getVictoryCards()[1].count, expected_count);

    EXPECT_EQ(board.getVictoryCards()[2].card, "Province");
    EXPECT_EQ(board.getVictoryCards()[2].count, expected_count);
}

// Test for counting empty piles
TEST(BoardTest, CountEmptyPiles)
{
    // Setup
    auto kingdom_cards = get_valid_kingdom_cards();

    size_t player_count = 2;
    TestableSharedBoard board(kingdom_cards, player_count);

    // Manually set counts for kingdom cards
    auto &kingdom_piles = board.getKingdomCards();
    for ( auto &pile : kingdom_piles ) {
        if ( pile.card == "Village" || pile.card == "Smithy" || pile.card == "Market" )
            pile.count = 0;
        else
            pile.count = 10; // Default count
    }

    // Manually set counts for victory cards
    auto &victory_piles = board.getVictoryCards();
    for ( auto &pile : victory_piles ) {
        if ( pile.card == "Estate" )
            pile.count = 5;
        else if ( pile.card == "Duchy" )
            pile.count = 0;
        else if ( pile.card == "Province" )
            pile.count = 3;
    }

    // Manually set counts for treasure cards
    auto &treasure_piles = board.getTreasureCards();
    for ( auto &pile : treasure_piles ) {
        if ( pile.card == "Copper" )
            pile.count = 0;
        else if ( pile.card == "Silver" )
            pile.count = 40;
        else if ( pile.card == "Gold" )
            pile.count = 30;
    }

    size_t empty_piles = board.countEmptyPiles();

    // Empty piles: Village, Mine, Moat, Duchy, Copper => total 5 empty piles
    EXPECT_EQ(empty_piles, 5);
}

// Test for checking if the game is over when the Province pile is empty
TEST(BoardTest, IsGameOverProvinceEmpty)
{
    // Setup
    auto kingdom_cards = get_valid_kingdom_cards();

    size_t player_count = 2;
    TestableSharedBoard board(kingdom_cards, player_count);

    board.initialise_victory_cards(player_count);

    // Set Province count to 0
    auto &victory_piles = board.getVictoryCards();
    auto it = std::find_if(victory_piles.begin(), victory_piles.end(),
                           [](shared::Pile &pile) { return pile.card == "Province"; });

    ASSERT_NE(it, victory_piles.end());
    it->count = 0;

    // Check if the game is over
    EXPECT_TRUE(board.isGameOver());
}

// Test for checking if the game is over when three piles are empty
TEST(BoardTest, IsGameOverThreeEmptyPiles)
{
    // Setup
    auto kingdom_cards = get_valid_kingdom_cards();

    size_t player_count = 2;
    TestableSharedBoard board(kingdom_cards, player_count);

    // Manually set counts for kingdom cards
    auto &kingdom_piles = board.getKingdomCards();
    for ( auto &pile : kingdom_piles ) {
        if ( pile.card == "Smithy" || pile.card == "Village" || pile.card == "Market" )
            pile.count = 0;
        else
            pile.count = 10; // Default count
    }

    // Manually set counts for victory cards
    auto &victory_piles = board.getVictoryCards();
    for ( auto &pile : victory_piles ) {
        pile.count = 5; // Default counts
    }

    // Manually set counts for treasure cards
    auto &treasure_piles = board.getTreasureCards();
    for ( auto &pile : treasure_piles ) {
        if ( pile.card == "Copper" )
            pile.count = 46;
        else if ( pile.card == "Silver" )
            pile.count = 40;
        else if ( pile.card == "Gold" )
            pile.count = 30;
    }

    // Since three kingdom card piles are empty, the game should be over
    EXPECT_TRUE(board.isGameOver());
}

// Test for checking that the game is not over when conditions are not met
TEST(BoardTest, IsGameOverNotOver)
{
    // Setup
    auto kingdom_cards = get_valid_kingdom_cards();

    size_t player_count = 2;
    TestableSharedBoard board(kingdom_cards, player_count);

    // Manually set counts for kingdom cards
    auto &kingdom_piles = board.getKingdomCards();
    for ( auto &pile : kingdom_piles ) {
        pile.count = 10; // None are empty
    }

    // Manually set counts for victory cards
    auto &victory_piles = board.getVictoryCards();
    for ( auto &pile : victory_piles ) {
        pile.count = 5; // Province pile is not empty
    }

    // Manually set counts for treasure cards
    auto &treasure_piles = board.getTreasureCards();
    for ( auto &pile : treasure_piles ) {
        if ( pile.card == "Copper" )
            pile.count = 46;
        else if ( pile.card == "Silver" )
            pile.count = 40;
        else if ( pile.card == "Gold" )
            pile.count = 30;
    }

    // No empty province pile and less than three empty piles
    EXPECT_FALSE(board.isGameOver());
}
