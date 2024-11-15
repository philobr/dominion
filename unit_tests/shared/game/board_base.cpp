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
    using shared::Board::getEmptyPilesCount;
    using shared::Board::initialiseTreasureCards;
    using shared::Board::initialiseVictoryCards;
    using shared::Board::isGameOver;

    // Expose protected member variables for testing access
    shared::Board::pile_container_t &getTreasureCards() { return treasure_cards; }
    shared::Board::pile_container_t &getVictoryCards() { return victory_cards; }
    shared::Board::pile_container_t &getKingdomCards() { return kingdom_cards; }
};

// Test for initializing victory cards with three players
TEST(BoardTest, InitialiseVictoryCardsThreePlayers)
{
    // Setup
    auto kingdom_cards = get_valid_kingdom_cards();

    size_t player_count = 3;
    TestableSharedBoard board(kingdom_cards, player_count);

    board.initialiseVictoryCards(player_count);

    ASSERT_EQ(board.getVictoryCards().size(), 4); // Estate, Duchy, Province, Curse

    size_t expected_count = 12; // For three or more players

    EXPECT_NE(board.getVictoryCards().find("Estate"), board.getVictoryCards().end());
    EXPECT_EQ(board.getVictoryCards().find("Estate")->count, expected_count);

    EXPECT_NE(board.getVictoryCards().find("Duchy"), board.getVictoryCards().end());
    EXPECT_EQ(board.getVictoryCards().find("Duchy")->count, expected_count);

    EXPECT_NE(board.getVictoryCards().find("Province"), board.getVictoryCards().end());
    EXPECT_EQ(board.getVictoryCards().find("Province")->count, expected_count);
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
        if ( pile.card_id == "Village" || pile.card_id == "Smithy" || pile.card_id == "Market" ) {
            pile.count = 0;
        } else {
            pile.count = 10; // Default count
        }
    }

    // Manually set counts for victory cards
    auto &victory_piles = board.getVictoryCards();
    for ( auto &pile : victory_piles ) {
        if ( pile.card_id == "Estate" ) {
            pile.count = 5;
        } else if ( pile.card_id == "Duchy" ) {
            pile.count = 0;
        } else if ( pile.card_id == "Province" ) {
            pile.count = 3;
        }
    }

    // Manually set counts for treasure cards
    auto &treasure_piles = board.getTreasureCards();
    for ( auto &pile : treasure_piles ) {
        if ( pile.card_id == "Copper" ) {
            pile.count = 0;
        } else if ( pile.card_id == "Silver" ) {
            pile.count = 40;
        } else if ( pile.card_id == "Gold" ) {
            pile.count = 30;
        }
    }

    size_t empty_piles = board.getEmptyPilesCount();

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

    board.initialiseVictoryCards(player_count);

    // Set Province count to 0
    auto &victory_piles = board.getVictoryCards();
    auto it = std::find_if(victory_piles.begin(), victory_piles.end(),
                           [](const auto &pile) { return pile.card_id == "Province"; });

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
        if ( pile.card_id == "Smithy" || pile.card_id == "Village" || pile.card_id == "Market" ) {
            pile.count = 0;
        } else {
            pile.count = 10; // Default count
        }
    }

    // Manually set counts for victory cards
    auto &victory_piles = board.getVictoryCards();
    for ( auto &pile : victory_piles ) {
        pile.count = 5; // Default counts
    }

    // Manually set counts for treasure cards
    auto &treasure_piles = board.getTreasureCards();
    for ( auto &pile : treasure_piles ) {
        if ( pile.card_id == "Copper" ) {
            pile.count = 46;
        } else if ( pile.card_id == "Silver" ) {
            pile.count = 40;
        } else if ( pile.card_id == "Gold" ) {
            pile.count = 30;
        }
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
        if ( pile.card_id == "Copper" ) {
            pile.count = 46;
        } else if ( pile.card_id == "Silver" ) {
            pile.count = 40;
        } else if ( pile.card_id == "Gold" ) {
            pile.count = 30;
        }
    }

    // No empty province pile and less than three empty piles
    EXPECT_FALSE(board.isGameOver());
}
