/**
 * @note We only test buy() and trash() here. The other functions are tested in unit_tests/shared/game/board_base.cpp
 */
#include <algorithm>
#include <gtest/gtest.h>
#include <string>
#include <vector>

#include <server/game/server_board.h>

#include <shared/game/cards/card_base.h>
#include <shared/game/game_state/board_base.h>
#include <shared/utils/test_helpers.h>

// ================================
// HELPERS
// ================================
#define EXPECT_PILE(pile_set, key, expected_count)                                                                     \
    do {                                                                                                               \
        auto it = (pile_set).find((key));                                                                              \
        EXPECT_NE(it, (pile_set).end());                                                                               \
        if ( it != (pile_set).end() ) {                                                                                \
            EXPECT_EQ(it->count, (expected_count));                                                                    \
        }                                                                                                              \
    } while ( 0 )

// ================================
// TESTABLE BASE CLASS
// ================================
class TestableServerBoard : public server::ServerBoard
{
public:
    // Inherit constructor from ServerBoard
    TestableServerBoard(const std::vector<shared::CardBase::id_t> &kingdom_cards, size_t player_count) :
        server::ServerBoard(kingdom_cards, player_count)
    {}

    // Expose protected methods as public for testing
    using server::ServerBoard::take;
    using server::ServerBoard::trashCard;

    // Accessors for protected data members
    const server::ServerBoard::pile_container_t &getKingdomCards() const { return kingdom_cards; }
    const server::ServerBoard::pile_container_t &getTreasureCards() const { return treasure_cards; }
    const server::ServerBoard::pile_container_t &getVictoryCards() const { return victory_cards; }
    const std::vector<shared::CardBase::id_t> &getTrash() const { return trash; }

    // Mutable accessors for testing purposes
    server::ServerBoard::pile_container_t &getMutableKingdomCards() { return kingdom_cards; }
    server::ServerBoard::pile_container_t &getMutableTreasureCards() { return treasure_cards; }
    server::ServerBoard::pile_container_t &getMutableVictoryCards() { return victory_cards; }
};

// ================================
// BEGIN TESTS
// ================================

// --------------------------------
// BuyCard Test
// --------------------------------
struct BuyCardTestParam
{
    size_t player_count;
    shared::CardBase::id_t card_to_buy;
    std::string pile_type; // "Kingdom", "Treasure", "Victory", "NonExistent"
    bool should_succeed;
    bool empty_pile; // Whether to empty the pile before buying
};

class ServerBoardBuyCardTest : public ::testing::TestWithParam<BuyCardTestParam>
{
protected:
    void SetUp() override
    {
        kingdom_cards = getValidKingdomCards();
        const auto &param = GetParam();
        player_count = param.player_count;
        board = new TestableServerBoard(kingdom_cards, player_count);

        if ( param.empty_pile ) {
            // Empty the specified pile
            if ( param.pile_type == "Kingdom" ) {
                auto &kingdom_piles = board->getMutableKingdomCards();
                auto it = kingdom_piles.find(param.card_to_buy);
                if ( it != kingdom_piles.end() ) {
                    it->count = 0;
                }
            } else if ( param.pile_type == "Treasure" ) {
                auto &treasure_piles = board->getMutableTreasureCards();
                auto it = treasure_piles.find(param.card_to_buy);
                if ( it != treasure_piles.end() ) {
                    it->count = 0;
                }
            } else if ( param.pile_type == "Victory" ) {
                auto &victory_piles = board->getMutableVictoryCards();
                auto it = victory_piles.find(param.card_to_buy);
                if ( it != victory_piles.end() ) {
                    it->count = 0;
                }
            }
        }
    }

    void TearDown() override { delete board; }

    TestableServerBoard *board;
    std::vector<shared::CardBase::id_t> kingdom_cards;
    size_t player_count;
};

TEST_P(ServerBoardBuyCardTest, BuyCardTest)
{
    const auto &param = GetParam();
    const auto &card_to_buy = param.card_to_buy;
    const auto &pile_type = param.pile_type;
    bool should_succeed = param.should_succeed;

    // Before buying, check the count if the card exists
    bool card_exists = false;
    size_t initial_count = 0;
    auto it = server::ServerBoard::pile_container_t::iterator();

    if ( pile_type == "Kingdom" ) {
        const auto &kingdom_piles = board->getKingdomCards();
        it = kingdom_piles.find(card_to_buy);
        if ( it != kingdom_piles.end() ) {
            card_exists = true;
            initial_count = it->count;
        }
    } else if ( pile_type == "Treasure" ) {
        const auto &treasure_piles = board->getTreasureCards();
        it = treasure_piles.find(card_to_buy);
        if ( it != treasure_piles.end() ) {
            card_exists = true;
            initial_count = it->count;
        }
    } else if ( pile_type == "Victory" ) {
        const auto &victory_piles = board->getVictoryCards();
        it = victory_piles.find(card_to_buy);
        if ( it != victory_piles.end() ) {
            card_exists = true;
            initial_count = it->count;
        }
    }

    // Perform the buy operation
    if ( should_succeed ) {
        EXPECT_NO_THROW(board->tryTake(card_to_buy));
    } else {
        EXPECT_THROW(board->tryTake(card_to_buy), exception::CardNotAvailable);
    }

    if ( should_succeed && card_exists ) {
        // After buying, check the count has decreased
        EXPECT_EQ(it->count, initial_count - 1);
    } else if ( !should_succeed && card_exists ) {
        // Count should remain the same
        EXPECT_EQ(it->count, initial_count);
    }
}

INSTANTIATE_TEST_SUITE_P(BuyCardTests, ServerBoardBuyCardTest,
                         ::testing::Values(
                                 // Successful buys from each pile
                                 BuyCardTestParam{2, "Village", "Kingdom", true, false},
                                 BuyCardTestParam{2, "Silver", "Treasure", true, false},
                                 BuyCardTestParam{2, "Estate", "Victory", true, false},

                                 // Attempt to buy a non-existent card
                                 BuyCardTestParam{2, "NonExistentCard", "", false, false},

                                 // Attempt to buy from an empty pile
                                 BuyCardTestParam{2, "Moat", "Kingdom", false, true},
                                 BuyCardTestParam{2, "Gold", "Treasure", false, true},
                                 BuyCardTestParam{2, "Duchy", "Victory", false, true}));

// --------------------------------
// TrashCard Test
// --------------------------------

struct TrashCardTestParam
{
    size_t player_count;
    std::vector<shared::CardBase::id_t> cards_to_trash;
};

class ServerBoardTrashCardTest : public ::testing::TestWithParam<TrashCardTestParam>
{
protected:
    void SetUp() override
    {
        kingdom_cards = getValidKingdomCards();
        player_count = GetParam().player_count;
        board = new TestableServerBoard(kingdom_cards, player_count);
    }

    void TearDown() override { delete board; }

    TestableServerBoard *board;
    std::vector<shared::CardBase::id_t> kingdom_cards;
    size_t player_count;
};

TEST_P(ServerBoardTrashCardTest, TrashCardTest)
{
    const auto &param = GetParam();
    const auto &cards_to_trash = param.cards_to_trash;

    // Before trashing, trash pile should be empty
    EXPECT_TRUE(board->getTrash().empty());

    // Perform the trash operations
    for ( const auto &card : cards_to_trash ) {
        board->trashCard(card);
    }

    // After trashing, trash pile should contain all the cards
    const auto &trash = board->getTrash();
    ASSERT_EQ(trash.size(), cards_to_trash.size());
    for ( size_t i = 0; i < cards_to_trash.size(); ++i ) {
        EXPECT_EQ(trash[i], cards_to_trash[i]);
    }
}

INSTANTIATE_TEST_SUITE_P(TrashCardTests, ServerBoardTrashCardTest,
                         ::testing::Values(TrashCardTestParam{2, {"Curse"}},
                                           TrashCardTestParam{2, {"Curse", "Estate", "Copper"}},
                                           TrashCardTestParam{3, {"Gold", "Silver", "Province"}},
                                           TrashCardTestParam{4, {"Village", "Smithy", "Market", "Moat"}}));

// --------------------------------
// BuyAllCards Test
// --------------------------------
TEST(ServerBoardTest, BuyAllCopiesOfCard)
{
    // Setup
    auto kingdom_cards = getValidKingdomCards();
    size_t player_count = 2;
    TestableServerBoard board(kingdom_cards, player_count);

    // Buy all copies of a kingdom card
    shared::CardBase::id_t card_to_buy = "Village";
    const size_t total_copies = shared::board_config::KINGDOM_CARD_COUNT;

    for ( size_t i = 0; i < total_copies; ++i ) {
        EXPECT_NO_THROW(board.tryTake(card_to_buy));
    }

    // Attempt to buy one more, should fail
    EXPECT_THROW(board.tryTake(card_to_buy), exception::CardNotAvailable);

    // Check that the pile count is zero
    const auto &kingdom_piles = board.getKingdomCards();
    auto it = kingdom_piles.find(card_to_buy);
    ASSERT_NE(it, kingdom_piles.end());
    EXPECT_EQ(it->count, 0);
}
