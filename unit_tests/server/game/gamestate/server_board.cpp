#include <algorithm>
#include <string>
#include <vector>
#include "gtest/gtest.h"

#include <server/game/game_state/server_board.h>

#include <shared/game/cards/card_base.h>
#include <shared/game/game_state/board_base.h>
#include <shared/utils/test_helpers.h>

void set_n_piles_to_empty(server::ServerBoard::pile_container_t &pile_container, size_t n)
{
    size_t i = 0;
    for ( auto pile_it = pile_container.begin(); i < n && pile_it != pile_container.end(); ++pile_it, ++i ) {
        pile_it->count = 0;
    }
}

#define EXPECT_PILE(pile_set, key, expected_count)                                                                     \
    do {                                                                                                               \
        auto it = (pile_set).find((key));                                                                              \
        EXPECT_NE(it, (pile_set).end());                                                                               \
        if ( it != (pile_set).end() ) {                                                                                \
            EXPECT_EQ(it->count, (expected_count));                                                                    \
        }                                                                                                              \
    } while ( 0 )

class TestableServerBoard : protected server::ServerBoard
{
public:
    // Inherit constructor from ServerBoard
    TestableServerBoard(const std::vector<shared::CardBase::id_t> &kingdom_cards, size_t player_count) :
        server::ServerBoard(kingdom_cards, player_count)
    {}

    // Expose private/protected methods for testing
    using server::ServerBoard::buy;
    using server::ServerBoard::getEmptyPilesCount;
    using server::ServerBoard::isGameOver;
    using server::ServerBoard::trash_card;

    // Accessors for protected data members
    const server::ServerBoard::pile_container_t &getKingdomCards() const { return kingdom_cards; }
    const server::ServerBoard::pile_container_t &getTreasureCards() const { return treasure_cards; }
    const server::ServerBoard::pile_container_t &getVictoryCards() const { return victory_cards; }
    const std::vector<shared::CardBase::id_t> &getTrash() const { return trash; }

    // Mutable accessors for testing purposes
    server::ServerBoard::pile_container_t &getMutableKingdomCards() { return kingdom_cards; }
    server::ServerBoard::pile_container_t &getMutableTreasureCards() { return treasure_cards; }
    server::ServerBoard::pile_container_t &getMutableVictoryCards() { return victory_cards; }
    std::vector<shared::CardBase::id_t> &getMutableTrash() { return trash; }
};

class ServerBoardParamTest : public ::testing::TestWithParam<size_t>
{
protected:
    void SetUp() override { kingdom_cards = get_valid_kingdom_cards(); }

    std::vector<shared::CardBase::id_t> kingdom_cards;
};

TEST_P(ServerBoardParamTest, ConstructorInitializesPilesForDifferentPlayerCounts)
{
    size_t player_count = GetParam();

    TestableServerBoard board(kingdom_cards, player_count);

    // check victory cards
    const auto &victory_cards = board.getVictoryCards();
    ASSERT_EQ(victory_cards.size(), 4); // Estate, Duchy, Province, Curse

    const size_t expected_victory_count = (player_count == 2) ? 8 : 12;
    EXPECT_PILE(victory_cards, "Estate", expected_victory_count);
    EXPECT_PILE(victory_cards, "Duchy", expected_victory_count);
    EXPECT_PILE(victory_cards, "Province", (player_count == 2) ? 8 : 12);

    // check treasure cards
    const auto &treasure_cards = board.getTreasureCards();
    ASSERT_EQ(treasure_cards.size(), 3); // Copper, Silver, Gold

    const size_t expected_copper_count = 60 - (player_count * 7);
    EXPECT_PILE(treasure_cards, "Copper", expected_copper_count);
    EXPECT_PILE(treasure_cards, "Silver", 40); // default val
    EXPECT_PILE(treasure_cards, "Gold", 30); // default val

    // check kingdom cards
    const auto &kingdom_piles = board.getKingdomCards();
    ASSERT_EQ(kingdom_piles.size(), kingdom_cards.size());

    for ( const auto &card_id : kingdom_cards ) {
        EXPECT_PILE(kingdom_piles, card_id, 10);
    }
}

INSTANTIATE_TEST_SUITE_P(PlayerCountTests, ServerBoardParamTest, ::testing::Values(2, 3, 4));

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
                           [&card_to_buy](const shared::Pile &pile) { return pile.card_id == card_to_buy; });
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
                           [&card_to_buy](const shared::Pile &pile) { return pile.card_id == card_to_buy; });
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
                           [&card_to_buy](const shared::Pile &pile) { return pile.card_id == card_to_buy; });
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
                           [&card_to_buy](const shared::Pile &pile) { return pile.card_id == card_to_buy; });
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
                           [](const shared::Pile &pile) { return pile.card_id == "Province"; });
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
    set_n_piles_to_empty(kingdom_piles, 3);

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

    set_n_piles_to_empty(kingdom_piles, 3);

    // Count empty piles
    size_t empty_piles = board.getEmptyPilesCount();
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
    EXPECT_EQ(treasure_piles[0].card_id, "Copper");
    EXPECT_EQ(treasure_piles[1].card_id, "Silver");
    EXPECT_EQ(treasure_piles[2].card_id, "Gold");

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
    EXPECT_EQ(victory_piles[0].card_id, "Estate");
    EXPECT_EQ(victory_piles[1].card_id, "Duchy");
    EXPECT_EQ(victory_piles[2].card_id, "Province");

    // Counts should be 8 for 2 players
    size_t expected_count = 8;
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
