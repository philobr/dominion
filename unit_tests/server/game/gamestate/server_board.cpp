#include <algorithm>
#include <string>
#include <vector>
#include "gtest/gtest.h"

#include <server/game/game_state/server_board.h>

#include <shared/game/cards/card_base.h>
#include <shared/game/game_state/board_base.h>
#include <shared/utils/test_helpers.h>

class TestableServerBoard : protected server::ServerBoard
{
public:
    // Inherit constructor from ServerBoard
    TestableServerBoard(const std::vector<shared::CardBase::id_t> &kingdom_cards, size_t player_count) :
        server::ServerBoard(kingdom_cards, player_count)
    {}

    // Expose private/protected methods for testing
    using server::ServerBoard::buy;
    using server::ServerBoard::countEmptyPiles;
    using server::ServerBoard::isGameOver;
    using server::ServerBoard::trash_card;

    // Accessors for protected data members
    const std::vector<shared::Pile> &getKingdomCards() const { return kingdom_cards; }
    const std::vector<shared::Pile> &getTreasureCards() const { return treasure_cards; }
    const std::vector<shared::Pile> &getVictoryCards() const { return victory_cards; }
    const std::vector<shared::CardBase::id_t> &getTrash() const { return trash; }

    // Mutable accessors for testing purposes
    std::vector<shared::Pile> &getMutableKingdomCards() { return kingdom_cards; }
    std::vector<shared::Pile> &getMutableTreasureCards() { return treasure_cards; }
    std::vector<shared::Pile> &getMutableVictoryCards() { return victory_cards; }
    std::vector<shared::CardBase::id_t> &getMutableTrash() { return trash; }
};

TEST(ServerBoardTest, ConstructorInitializesPiles)
{
    // Setup
    auto kingdom_cards = get_valid_kingdom_cards();

    size_t player_count = 2;

    TestableServerBoard board(kingdom_cards, player_count);

    // Check that victory cards are initialized correctly
    size_t expected_victory_card_count = (player_count == 2) ? 8 : 12;

    const auto &victory_cards = board.getVictoryCards();

    ASSERT_EQ(victory_cards.size(), 4); // Estate, Duchy, Province, Curse
    EXPECT_EQ(victory_cards[0].card, "Estate");
    EXPECT_EQ(victory_cards[0].count, expected_victory_card_count);

    EXPECT_EQ(victory_cards[1].card, "Duchy");
    EXPECT_EQ(victory_cards[1].count, expected_victory_card_count);

    EXPECT_EQ(victory_cards[2].card, "Province");
    EXPECT_EQ(victory_cards[2].count, expected_victory_card_count);

    // Check that treasure cards are initialized correctly
    const auto &treasure_cards = board.getTreasureCards();
    ASSERT_EQ(treasure_cards.size(), 3); // Copper, Silver, Gold
    EXPECT_EQ(treasure_cards[0].card, "Copper");
    EXPECT_EQ(treasure_cards[1].card, "Silver");
    EXPECT_EQ(treasure_cards[2].card, "Gold");

    // Check that kingdom cards are initialized correctly
    const auto &kingdom_piles = board.getKingdomCards();
    ASSERT_EQ(kingdom_piles.size(), kingdom_cards.size());
    for ( size_t i = 0; i < kingdom_cards.size(); ++i ) {
        EXPECT_EQ(kingdom_piles[i].card, kingdom_cards[i]);
        // Assuming each kingdom card pile starts with 10 cards
        EXPECT_EQ(kingdom_piles[i].count, 10);
    }
}

TEST(ServerBoardTest, BuyCardFromKingdomPile)
{
    // Setup
    auto kingdom_cards = get_valid_kingdom_cards();

    size_t player_count = 2;

    TestableServerBoard board(kingdom_cards, player_count);

    // Attempt to buy a kingdom card
    shared::CardBase::id_t card_to_buy = "Village";

    // Before buying, check the count
    const auto &kingdom_piles = board.getKingdomCards();
    auto it = std::find_if(kingdom_piles.begin(), kingdom_piles.end(),
                           [&card_to_buy](const shared::Pile &pile) { return pile.card == card_to_buy; });
    ASSERT_NE(it, kingdom_piles.end());
    EXPECT_EQ(it->count, 10);

    // Perform the buy operation
    bool success = board.buy(card_to_buy);
    EXPECT_TRUE(success);

    // After buying, check the count has decreased
    EXPECT_EQ(it->count, 9);
}

TEST(ServerBoardTest, BuyCardFromTreasurePile)
{
    // Setup
    auto kingdom_cards = get_valid_kingdom_cards();

    size_t player_count = 2;

    TestableServerBoard board(kingdom_cards, player_count);

    // Attempt to buy a treasure card
    shared::CardBase::id_t card_to_buy = "Silver";

    // Before buying, check the count
    const auto &treasure_piles = board.getTreasureCards();
    auto it = std::find_if(treasure_piles.begin(), treasure_piles.end(),
                           [&card_to_buy](const shared::Pile &pile) { return pile.card == card_to_buy; });
    ASSERT_NE(it, treasure_piles.end());
    size_t initial_count = it->count;

    // Perform the buy operation
    bool success = board.buy(card_to_buy);
    EXPECT_TRUE(success);

    // After buying, check the count has decreased
    EXPECT_EQ(it->count, initial_count - 1);
}

TEST(ServerBoardTest, BuyCardFromVictoryPile)
{
    // Setup
    auto kingdom_cards = get_valid_kingdom_cards();

    size_t player_count = 2;

    TestableServerBoard board(kingdom_cards, player_count);

    // Attempt to buy a victory card
    shared::CardBase::id_t card_to_buy = "Estate";

    // Before buying, check the count
    const auto &victory_piles = board.getVictoryCards();
    auto it = std::find_if(victory_piles.begin(), victory_piles.end(),
                           [&card_to_buy](const shared::Pile &pile) { return pile.card == card_to_buy; });
    ASSERT_NE(it, victory_piles.end());
    size_t initial_count = it->count;

    // Perform the buy operation
    bool success = board.buy(card_to_buy);
    EXPECT_TRUE(success);

    // After buying, check the count has decreased
    EXPECT_EQ(it->count, initial_count - 1);
}

TEST(ServerBoardTest, BuyCardNotInPiles)
{
    // Setup
    auto kingdom_cards = get_valid_kingdom_cards();

    size_t player_count = 2;

    TestableServerBoard board(kingdom_cards, player_count);

    // Attempt to buy a card that doesn't exist
    shared::CardBase::id_t card_to_buy = "NonExistentCard";

    // Perform the buy operation
    bool success = board.buy(card_to_buy);
    EXPECT_FALSE(success);
}

TEST(ServerBoardTest, BuyCardFromEmptyPile)
{
    // Setup
    auto kingdom_cards = get_valid_kingdom_cards();

    size_t player_count = 2;

    TestableServerBoard board(kingdom_cards, player_count);

    // Empty a pile first
    shared::CardBase::id_t card_to_buy = "Moat";

    // Find the pile and set count to 0
    auto &kingdom_piles = board.getMutableKingdomCards();
    auto it = std::find_if(kingdom_piles.begin(), kingdom_piles.end(),
                           [&card_to_buy](shared::Pile &pile) { return pile.card == card_to_buy; });
    ASSERT_NE(it, kingdom_piles.end());
    it->count = 0;

    // Attempt to buy the card
    bool success = board.buy(card_to_buy);
    EXPECT_FALSE(success);
}

TEST(ServerBoardTest, TrashCard)
{
    // Setup
    auto kingdom_cards = get_valid_kingdom_cards();

    size_t player_count = 2;

    TestableServerBoard board(kingdom_cards, player_count);

    // Trash a card
    shared::CardBase::id_t card_to_trash = "Curse"; // Assuming "Curse" is a card in the game

    // Before trashing, trash pile should be empty
    EXPECT_TRUE(board.getTrash().empty());

    // Perform the trash operation
    board.trash_card(card_to_trash);

    // After trashing, trash pile should contain the card
    const auto &trash = board.getTrash();
    ASSERT_EQ(trash.size(), 1);
    EXPECT_EQ(trash[0], card_to_trash);
}

TEST(ServerBoardTest, IsGameOverProvinceEmpty)
{
    // Setup
    auto kingdom_cards = get_valid_kingdom_cards();

    size_t player_count = 2;

    TestableServerBoard board(kingdom_cards, player_count);

    // Set Province pile to 0
    auto &victory_piles = board.getMutableVictoryCards();
    auto it = std::find_if(victory_piles.begin(), victory_piles.end(),
                           [](shared::Pile &pile) { return pile.card == "Province"; });
    ASSERT_NE(it, victory_piles.end());
    it->count = 0;

    // Check if game is over
    EXPECT_TRUE(board.isGameOver());
}

TEST(ServerBoardTest, IsGameOverThreeEmptyPiles)
{
    // Setup
    auto kingdom_cards = get_valid_kingdom_cards();

    size_t player_count = 2;

    TestableServerBoard board(kingdom_cards, player_count);

    // Empty three kingdom card piles
    auto &kingdom_piles = board.getMutableKingdomCards();
    kingdom_piles[0].count = 0;
    kingdom_piles[1].count = 0;
    kingdom_piles[2].count = 0;

    // Check if game is over
    EXPECT_TRUE(board.isGameOver());
}

TEST(ServerBoardTest, IsGameOverNotOver)
{
    // Setup
    auto kingdom_cards = get_valid_kingdom_cards();

    size_t player_count = 2;

    TestableServerBoard board(kingdom_cards, player_count);

    // Ensure that less than three piles are empty and Province pile is not empty
    // Check if game is over
    EXPECT_FALSE(board.isGameOver());
}

TEST(ServerBoardTest, CountEmptyPiles)
{
    // Setup
    auto kingdom_cards = get_valid_kingdom_cards();

    size_t player_count = 2;

    TestableServerBoard board(kingdom_cards, player_count);

    // Empty some piles
    auto &kingdom_piles = board.getMutableKingdomCards();
    auto &victory_piles = board.getMutableVictoryCards();
    auto &treasure_piles = board.getMutableTreasureCards();

    kingdom_piles[0].count = 0; // 1 empty pile
    victory_piles[1].count = 0; // 2 empty piles
    treasure_piles[2].count = 0; // 3 empty piles

    // Count empty piles
    size_t empty_piles = board.countEmptyPiles();
    EXPECT_EQ(empty_piles, 3);
}

TEST(ServerBoardTest, InitialiseTreasureCards)
{
    // Create a new ServerBoard instance to test initialization
    auto kingdom_cards = get_valid_kingdom_cards();

    size_t player_count = 2;
    TestableServerBoard board(kingdom_cards, player_count);

    // Verify treasure cards
    const auto &treasure_piles = board.getTreasureCards();
    ASSERT_EQ(treasure_piles.size(), 3);
    EXPECT_EQ(treasure_piles[0].card, "Copper");
    EXPECT_EQ(treasure_piles[1].card, "Silver");
    EXPECT_EQ(treasure_piles[2].card, "Gold");

    // Assuming counts are set as per Dominion rules
    // For simplicity, check that counts are greater than zero
    EXPECT_GT(treasure_piles[0].count, 0);
    EXPECT_GT(treasure_piles[1].count, 0);
    EXPECT_GT(treasure_piles[2].count, 0);
}

TEST(ServerBoardTest, InitialiseVictoryCards)
{
    // Create a new ServerBoard instance to test initialization
    auto kingdom_cards = get_valid_kingdom_cards();

    size_t player_count = 2;
    TestableServerBoard board(kingdom_cards, player_count);

    // Verify victory cards
    const auto &victory_piles = board.getVictoryCards();
    ASSERT_EQ(victory_piles.size(), 4); // Estate, Duchy, Province, Curse
    EXPECT_EQ(victory_piles[0].card, "Estate");
    EXPECT_EQ(victory_piles[1].card, "Duchy");
    EXPECT_EQ(victory_piles[2].card, "Province");

    // Counts should be 8 for 2 players
    size_t expected_count = (player_count == 2) ? 8 : 12;
    EXPECT_EQ(victory_piles[0].count, expected_count);
    EXPECT_EQ(victory_piles[1].count, expected_count);
    EXPECT_EQ(victory_piles[2].count, expected_count);
}

TEST(ServerBoardTest, TrashCardMultiple)
{
    // Setup
    auto kingdom_cards = get_valid_kingdom_cards();

    size_t player_count = 2;
    TestableServerBoard board(kingdom_cards, player_count);

    // Trash multiple cards
    std::vector<shared::CardBase::id_t> cards_to_trash = {"Curse", "Estate", "Copper"};

    for ( const auto &card : cards_to_trash ) {
        board.trash_card(card);
    }

    // Check that trash pile contains all the cards
    const auto &trash = board.getTrash();
    ASSERT_EQ(trash.size(), cards_to_trash.size());
    for ( size_t i = 0; i < cards_to_trash.size(); ++i ) {
        EXPECT_EQ(trash[i], cards_to_trash[i]);
    }
}

TEST(ServerBoardTest, ConstructorAssertTest)
{
    size_t valid_players = 3;
    auto kingdom_cards = get_valid_kingdom_cards();

    ASSERT_DEATH({ server::ServerBoard::make(kingdom_cards, 1); }, ""); // not enough players
    ASSERT_DEATH({ server::ServerBoard::make(kingdom_cards, 5); }, ""); // too many players

    kingdom_cards.push_back("new_card");
    ASSERT_DEATH({ server::ServerBoard::make(kingdom_cards, valid_players); }, ""); // too many cards

    kingdom_cards.pop_back();
    kingdom_cards.pop_back();
    ASSERT_DEATH({ server::ServerBoard::make(kingdom_cards, valid_players); }, ""); // not enough cards
}
