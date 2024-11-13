#include <string>
#include <vector>
#include "gtest/gtest.h"

#include <server/game/game_state/server_board.h>

// Include the header files for the classes we are testing
#include <shared/game/cards/card_base.h> // Adjust the path according to your project structure
#include <shared/game/game_state/board_base.h> // Adjust the path according to your project structure

// Begin test cases
class ServerBoardTest : public ::testing::Test
{
protected:
    // cppcheck-suppress unusedFunction
    void SetUp() override
    {
        // Initialize common test data
        player_count = 2;

        // Define some kingdom card IDs (assuming card IDs are strings)
        kingdom_cards = {"Smithy",  "Village",  "Market", "Mine",   "Moat",
                         "Militia", "Workshop", "Cellar", "Chapel", "Woodcutter"};

        // Create a ServerBoard instance
        board = std::make_unique<server::ServerBoard>(kingdom_cards, player_count);
    }

    // cppcheck-suppress unusedFunction
    void TearDown() override
    {
        // Clean up if necessary
    }

    size_t player_count;
    std::vector<shared::CardBase::id_t> kingdom_cards;
    std::unique_ptr<server::ServerBoard> board;
};

TEST_F(ServerBoardTest, ConstructorInitializesPiles)
{
    // Check that victory cards are initialized correctly
    size_t expected_victory_card_count = (player_count == 2) ? 8 : 12;

    ASSERT_EQ(board->victory_cards.size(), 4); // Estate, Duchy, Province, (+curse)
    EXPECT_EQ(board->victory_cards[0].card, "Estate");
    EXPECT_EQ(board->victory_cards[0].count, expected_victory_card_count);

    EXPECT_EQ(board->victory_cards[1].card, "Duchy");
    EXPECT_EQ(board->victory_cards[1].count, expected_victory_card_count);

    EXPECT_EQ(board->victory_cards[2].card, "Province");
    EXPECT_EQ(board->victory_cards[2].count, expected_victory_card_count);

    // Check that treasure cards are initialized correctly
    ASSERT_EQ(board->treasure_cards.size(), 3); // Copper, Silver, Gold
    EXPECT_EQ(board->treasure_cards[0].card, "Copper");
    EXPECT_EQ(board->treasure_cards[1].card, "Silver");
    EXPECT_EQ(board->treasure_cards[2].card, "Gold");

    // Check that kingdom cards are initialized correctly
    ASSERT_EQ(board->kingdom_cards.size(), kingdom_cards.size());
    for ( size_t i = 0; i < kingdom_cards.size(); ++i ) {
        EXPECT_EQ(board->kingdom_cards[i].card, kingdom_cards[i]);
        // Assuming each kingdom card pile starts with 10 cards
        EXPECT_EQ(board->kingdom_cards[i].count, 10);
    }
}

TEST_F(ServerBoardTest, BuyCardFromKingdomPile)
{
    // Attempt to buy a kingdom card
    std::string card_to_buy = "Village";

    // Before buying, check the count
    auto it = std::find_if(board->kingdom_cards.begin(), board->kingdom_cards.end(),
                           [&card_to_buy](const shared::Pile &pile) { return pile.card == card_to_buy; });
    ASSERT_NE(it, board->kingdom_cards.end());
    EXPECT_EQ(it->count, 10);

    // Perform the buy operation
    bool success = board->buy(card_to_buy);
    EXPECT_TRUE(success);

    // After buying, check the count has decreased
    EXPECT_EQ(it->count, 9);
}

TEST_F(ServerBoardTest, BuyCardFromTreasurePile)
{
    // Attempt to buy a treasure card
    std::string card_to_buy = "Silver";

    // Before buying, check the count
    auto it = std::find_if(board->treasure_cards.begin(), board->treasure_cards.end(),
                           [&card_to_buy](const shared::Pile &pile) { return pile.card == card_to_buy; });
    ASSERT_NE(it, board->treasure_cards.end());
    size_t initial_count = it->count;

    // Perform the buy operation
    bool success = board->buy(card_to_buy);
    EXPECT_TRUE(success);

    // After buying, check the count has decreased
    EXPECT_EQ(it->count, initial_count - 1);
}

TEST_F(ServerBoardTest, BuyCardFromVictoryPile)
{
    // Attempt to buy a victory card
    std::string card_to_buy = "Estate";

    // Before buying, check the count
    auto it = std::find_if(board->victory_cards.begin(), board->victory_cards.end(),
                           [&card_to_buy](const shared::Pile &pile) { return pile.card == card_to_buy; });
    ASSERT_NE(it, board->victory_cards.end());
    size_t initial_count = it->count;

    // Perform the buy operation
    bool success = board->buy(card_to_buy);
    EXPECT_TRUE(success);

    // After buying, check the count has decreased
    EXPECT_EQ(it->count, initial_count - 1);
}

TEST_F(ServerBoardTest, BuyCardNotInPiles)
{
    // Attempt to buy a card that doesn't exist
    std::string card_to_buy = "NonExistentCard";

    // Perform the buy operation
    bool success = board->buy(card_to_buy);
    EXPECT_FALSE(success);
}

TEST_F(ServerBoardTest, BuyCardFromEmptyPile)
{
    // Empty a pile first
    std::string card_to_buy = "Moat";

    // Find the pile and set count to 0
    auto it = std::find_if(board->kingdom_cards.begin(), board->kingdom_cards.end(),
                           [&card_to_buy](const shared::Pile &pile) { return pile.card == card_to_buy; });
    ASSERT_NE(it, board->kingdom_cards.end());
    it->count = 0;

    // Attempt to buy the card
    bool success = board->buy(card_to_buy);
    EXPECT_FALSE(success);
}

TEST_F(ServerBoardTest, TrashCard)
{
    // Trash a card
    std::string card_to_trash = "Curse"; // Assuming "Curse" is a card in the game

    // Before trashing, trash pile should be empty
    EXPECT_TRUE(board->trash.empty());

    // Perform the trash operation
    board->trash_card(card_to_trash);

    // After trashing, trash pile should contain the card
    ASSERT_EQ(board->trash.size(), 1);
    EXPECT_EQ(board->trash[0], card_to_trash);
}

TEST_F(ServerBoardTest, IsGameOverProvinceEmpty)
{
    // Set Province pile to 0
    auto it = std::find_if(board->victory_cards.begin(), board->victory_cards.end(),
                           [](const shared::Pile &pile) { return pile.card == "Province"; });
    ASSERT_NE(it, board->victory_cards.end());
    it->count = 0;

    // Check if game is over
    EXPECT_TRUE(board->isGameOver());
}

TEST_F(ServerBoardTest, IsGameOverThreeEmptyPiles)
{
    // Empty three kingdom card piles
    board->kingdom_cards[0].count = 0;
    board->kingdom_cards[1].count = 0;
    board->kingdom_cards[2].count = 0;

    // Check if game is over
    EXPECT_TRUE(board->isGameOver());
}

TEST_F(ServerBoardTest, IsGameOverNotOver)
{
    // Ensure that less than three piles are empty and Province pile is not empty
    // Check if game is over
    EXPECT_FALSE(board->isGameOver());
}

TEST_F(ServerBoardTest, CountEmptyPiles)
{
    // Empty some piles
    board->kingdom_cards[0].count = 0; // 1 empty pile
    board->victory_cards[1].count = 0; // 2 empty piles
    board->treasure_cards[2].count = 0; // 3 empty piles

    // Count empty piles
    size_t empty_piles = board->countEmptyPiles();
    EXPECT_EQ(empty_piles, 3);
}

TEST_F(ServerBoardTest, InitialiseTreasureCards)
{
    // Create a new ServerBoard instance to test initialization
    server::ServerBoard new_board(kingdom_cards, player_count);

    // Verify treasure cards
    ASSERT_EQ(new_board.treasure_cards.size(), 3);
    EXPECT_EQ(new_board.treasure_cards[0].card, "Copper");
    EXPECT_EQ(new_board.treasure_cards[1].card, "Silver");
    EXPECT_EQ(new_board.treasure_cards[2].card, "Gold");

    // Assuming counts are set as per Dominion rules
    // For simplicity, check that counts are greater than zero
    EXPECT_GT(new_board.treasure_cards[0].count, 0);
    EXPECT_GT(new_board.treasure_cards[1].count, 0);
    EXPECT_GT(new_board.treasure_cards[2].count, 0);
}

TEST_F(ServerBoardTest, InitialiseVictoryCards)
{
    // Create a new ServerBoard instance to test initialization
    server::ServerBoard new_board(kingdom_cards, player_count);

    // Verify victory cards
    ASSERT_EQ(new_board.victory_cards.size(), 4); // estate, duchy, province, (+curse)
    EXPECT_EQ(new_board.victory_cards[0].card, "Estate");
    EXPECT_EQ(new_board.victory_cards[1].card, "Duchy");
    EXPECT_EQ(new_board.victory_cards[2].card, "Province");

    // Counts should be 8 for 2 players
    size_t expected_count = (player_count == 2) ? 8 : 12;
    EXPECT_EQ(new_board.victory_cards[0].count, expected_count);
    EXPECT_EQ(new_board.victory_cards[1].count, expected_count);
    EXPECT_EQ(new_board.victory_cards[2].count, expected_count);
}

TEST_F(ServerBoardTest, PlayedCards)
{
    // Initially, played_cards should be empty
    EXPECT_TRUE(board->played_cards.empty());

    // Simulate playing a card
    std::string played_card = "Village";
    board->played_cards.push_back(played_card);

    // Check that played_cards contains the card
    ASSERT_EQ(board->played_cards.size(), 1);
    EXPECT_EQ(board->played_cards[0], played_card);
}

TEST_F(ServerBoardTest, TrashCardMultiple)
{
    // Trash multiple cards
    std::vector<std::string> cards_to_trash = {"Curse", "Estate", "Copper"};

    for ( const auto &card : cards_to_trash ) {
        board->trash_card(card);
    }

    // Check that trash pile contains all the cards
    ASSERT_EQ(board->trash.size(), cards_to_trash.size());
    for ( size_t i = 0; i < cards_to_trash.size(); ++i ) {
        EXPECT_EQ(board->trash[i], cards_to_trash[i]);
    }
}
