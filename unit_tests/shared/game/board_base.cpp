/**
 * @note The pile sizes, card counts etc are hardcoded on purpose. This way we notice if we change them by accident.
 */
#include <algorithm>
#include <string>
#include <vector>
#include <gtest/gtest.h>

#include <shared/game/cards/card_base.h>
#include <shared/game/game_state/board_base.h>
#include <shared/utils/test_helpers.h>

TEST(PileTest, Pile2WayJsonConversion)
{
    shared::Pile expected("Militia", 10);
    auto json = expected.toJson();
    std::unique_ptr<shared::Pile> actual = shared::Pile::fromJson(json);
    EXPECT_EQ(*actual, expected);
}

TEST(BoardJsonTest, Board2WayJsonConversion)
{
    std::vector<shared::CardBase::id_t> kingdom_cards = get_valid_kingdom_cards();
    shared::Board::ptr_t expected = shared::Board::make(kingdom_cards, 2);
    auto json = expected->toJson();
    shared::Board::ptr_t actual = shared::Board::fromJson(json);
    ASSERT_NE(actual, nullptr);
    EXPECT_EQ(*actual, *expected);
}

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

namespace expected_constants
{
    static constexpr size_t KINGDOM_CARD_COUNT = 10;

    static constexpr size_t TREASURE_COPPER_COUNT = 60;
    static constexpr size_t TREASURE_SILVER_COUNT = 40;
    static constexpr size_t TREASURE_GOLD_COUNT = 30;

    static constexpr size_t VICTORY_CARDS_SMALL_GAME = 8; // for player count == 2
    static constexpr size_t VICTORY_CARDS_LARGE_GAME = 12; // for player count > 2
    static constexpr size_t CURSE_MULTIPLIER = 10; // 10 curse cards per player
}; // namespace expected_constants

// ================================
// TESTABLE BASE CLASS
// ================================
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
    shared::Pile &getCursePile() { return curse_card_pile; }
    std::vector<shared::CardBase::id_t> &getTrash() { return trash; }
};

// ================================
// BEGIN TESTS
// ================================

// --------------------------------
// Constructor Test
// --------------------------------
class BoardInitializationTest : public ::testing::TestWithParam<size_t>
{
protected:
    void SetUp() override
    {
        kingdom_cards = get_valid_kingdom_cards();
        player_count = GetParam();
        board = new TestableSharedBoard(kingdom_cards, player_count);
    }

    void TearDown() override { delete board; }

    TestableSharedBoard *board;
    std::vector<shared::CardBase::id_t> kingdom_cards;
    size_t player_count;
};

TEST_P(BoardInitializationTest, InitializePiles)
{
    // Check treasure cards
    const auto &treasure_cards = board->getTreasureCards();
    ASSERT_EQ(treasure_cards.size(), 3); // Copper, Silver, Gold

    size_t expected_copper_count = expected_constants::TREASURE_COPPER_COUNT - (7 * player_count);
    EXPECT_PILE(treasure_cards, "Copper", expected_copper_count);
    EXPECT_PILE(treasure_cards, "Silver", expected_constants::TREASURE_SILVER_COUNT);
    EXPECT_PILE(treasure_cards, "Gold", expected_constants::TREASURE_GOLD_COUNT);

    // Check victory cards
    const auto &victory_cards = board->getVictoryCards();
    ASSERT_EQ(victory_cards.size(), 3); // Estate, Duchy, Province

    size_t expected_victory_count = (player_count == 2) ? expected_constants::VICTORY_CARDS_SMALL_GAME
                                                        : expected_constants::VICTORY_CARDS_LARGE_GAME;
    EXPECT_PILE(victory_cards, "Estate", expected_victory_count);
    EXPECT_PILE(victory_cards, "Duchy", expected_victory_count);
    EXPECT_PILE(victory_cards, "Province", expected_victory_count);

    const auto &curse_cards = board->getCursePile();
    const size_t expected_curse_count = expected_constants::CURSE_MULTIPLIER * (player_count - 1);
    EXPECT_EQ(curse_cards.card_id, "Curse");
    EXPECT_EQ(curse_cards.count, expected_curse_count);

    // Check kingdom cards
    const auto &kingdom_piles = board->getKingdomCards();
    ASSERT_EQ(kingdom_piles.size(), kingdom_cards.size());
    for ( const auto &card_id : kingdom_cards ) {
        EXPECT_PILE(kingdom_piles, card_id, expected_constants::KINGDOM_CARD_COUNT);
    }
}

INSTANTIATE_TEST_SUITE_P(PlayerCountTests, BoardInitializationTest, ::testing::Values(2, 3, 4));

// --------------------------------
// GameOver Test
// --------------------------------
struct GameOverTestParam
{
    size_t player_count;
    size_t empty_kingdom_piles;
    bool empty_province_pile;
    bool expect_game_over;
};

class BoardGameOverTest : public ::testing::TestWithParam<GameOverTestParam>
{
protected:
    void SetUp() override
    {
        kingdom_cards = get_valid_kingdom_cards();
        const auto &param = GetParam();
        player_count = param.player_count;
        board = new TestableSharedBoard(kingdom_cards, player_count);

        // Empty specified number of kingdom piles
        if ( param.empty_kingdom_piles > 0 ) {
            auto &kingdom_piles = board->getKingdomCards();
            set_n_piles_to_empty(kingdom_piles, param.empty_kingdom_piles);
        }

        // Empty the Province pile if required
        if ( param.empty_province_pile ) {
            const auto &victory_piles = board->getVictoryCards();
            auto it = victory_piles.find("Province");
            if ( it != victory_piles.end() ) {
                it->count = 0;
            }
        }
    }

    void TearDown() override { delete board; }

    TestableSharedBoard *board;
    std::vector<shared::CardBase::id_t> kingdom_cards;
    size_t player_count;
};

TEST_P(BoardGameOverTest, IsGameOver)
{
    const auto &param = GetParam();
    EXPECT_EQ(board->isGameOver(), param.expect_game_over);
}

INSTANTIATE_TEST_SUITE_P(GameOverTests, BoardGameOverTest,
                         ::testing::Values(GameOverTestParam{2, 0, true, true}, // Province pile empty
                                           GameOverTestParam{2, 3, false, true}, // Three kingdom piles empty
                                           GameOverTestParam{2, 2, false, false}, // Less than three kingdom piles empty
                                           GameOverTestParam{3, 0, false, false}, // Game not over
                                           GameOverTestParam{4, 3, false, true} // Three kingdom piles empty
                                           ));


// --------------------------------
// GetEmptyPiles Test
// --------------------------------
struct EmptyPilesTestParam
{
    size_t player_count;
    size_t empty_kingdom_piles;
    size_t empty_victory_piles;
    size_t empty_treasure_piles;
    size_t expected_empty_piles;
};

class BoardEmptyPilesTest : public ::testing::TestWithParam<EmptyPilesTestParam>
{
protected:
    void SetUp() override
    {
        kingdom_cards = get_valid_kingdom_cards();
        const auto &param = GetParam();
        player_count = param.player_count;
        board = new TestableSharedBoard(kingdom_cards, player_count);

        // Empty specified number of kingdom piles
        if ( param.empty_kingdom_piles > 0 ) {
            auto &kingdom_piles = board->getKingdomCards();
            set_n_piles_to_empty(kingdom_piles, param.empty_kingdom_piles);
        }

        // Empty specified number of victory piles
        if ( param.empty_victory_piles > 0 ) {
            auto &victory_piles = board->getVictoryCards();
            set_n_piles_to_empty(victory_piles, param.empty_victory_piles);
        }

        // Empty specified number of treasure piles
        if ( param.empty_treasure_piles > 0 ) {
            auto &treasure_piles = board->getTreasureCards();
            set_n_piles_to_empty(treasure_piles, param.empty_treasure_piles);
        }
    }

    void TearDown() override { delete board; }

    TestableSharedBoard *board;
    std::vector<shared::CardBase::id_t> kingdom_cards;
    size_t player_count;
};

TEST_P(BoardEmptyPilesTest, GetEmptyPilesCount)
{
    const auto &param = GetParam();
    EXPECT_EQ(board->getEmptyPilesCount(), param.expected_empty_piles);
}

INSTANTIATE_TEST_SUITE_P(EmptyPilesTests, BoardEmptyPilesTest,
                         ::testing::Values(EmptyPilesTestParam{2, 3, 0, 0, 3}, EmptyPilesTestParam{2, 0, 2, 0, 2},
                                           EmptyPilesTestParam{2, 0, 0, 1, 1}, EmptyPilesTestParam{3, 5, 2, 1, 8},
                                           EmptyPilesTestParam{4, 0, 0, 0, 0}));


// --------------------------------
// PileCreation Test
// --------------------------------
class PileCreationTest : public ::testing::TestWithParam<size_t>
{};

TEST(PileCreationTest, MakeKingdomCard)
{
    shared::CardBase::id_t card_id = "Village";
    shared::Pile pile = shared::Pile::makeKingdomCard(card_id);

    EXPECT_EQ(pile.card_id, card_id);
    EXPECT_EQ(pile.count, shared::BoardConfig::KINGDOM_CARD_COUNT);
}

TEST_P(PileCreationTest, MakePile)
{
    shared::CardBase::id_t card_id = "Copper";
    size_t pile_size = GetParam();
    shared::Pile pile = shared::Pile(card_id, pile_size);

    EXPECT_EQ(pile.card_id, card_id);
    EXPECT_EQ(pile.count, pile_size);
}

INSTANTIATE_TEST_SUITE_P(PileSizeTests, PileCreationTest, ::testing::Values(10, 20, 30, 40));

struct DuplicateKingdomCardsTestParam
{
    size_t player_count;
    std::vector<shared::CardBase::id_t> kingdom_cards;
    bool expect_exception;
};

class BoardDuplicateKingdomCardsTest : public ::testing::TestWithParam<DuplicateKingdomCardsTestParam>
{};

TEST_P(BoardDuplicateKingdomCardsTest, DuplicateKingdomCards)
{
    const auto &param = GetParam();

    if ( param.expect_exception ) {
        EXPECT_DEATH({ TestableSharedBoard(param.kingdom_cards, param.player_count); }, "");
    } else {
        EXPECT_NO_THROW({ TestableSharedBoard(param.kingdom_cards, param.player_count); });
    }
}

INSTANTIATE_TEST_SUITE_P(DuplicateKingdomCardsTests, BoardDuplicateKingdomCardsTest,
                         ::testing::Values(DuplicateKingdomCardsTestParam{2, get_valid_kingdom_cards(), false},
                                           DuplicateKingdomCardsTestParam{2,
                                                                          []
                                                                          {
                                                                              auto cards = get_valid_kingdom_cards();
                                                                              cards[1] =
                                                                                      cards[0]; // Introduce a duplicate
                                                                              return cards;
                                                                          }(),
                                                                          true}));


// --------------------------------
// InvalidPlayerCount Test
// --------------------------------
struct InvalidPlayerCountTestParam
{
    size_t player_count;
    bool expect_exception;
};

class BoardInvalidPlayerCountTest : public ::testing::TestWithParam<InvalidPlayerCountTestParam>
{};

TEST_P(BoardInvalidPlayerCountTest, InvalidPlayerCount)
{
    const auto &param = GetParam();
    auto kingdom_cards = get_valid_kingdom_cards();

    if ( param.expect_exception ) {
        EXPECT_DEATH({ TestableSharedBoard(kingdom_cards, param.player_count); }, "");
    } else {
        EXPECT_NO_THROW({ TestableSharedBoard(kingdom_cards, param.player_count); });
    }
}

INSTANTIATE_TEST_SUITE_P(InvalidPlayerCountTests, BoardInvalidPlayerCountTest,
                         ::testing::Values(InvalidPlayerCountTestParam{1, true}, // Less than minimum
                                           InvalidPlayerCountTestParam{2, false}, InvalidPlayerCountTestParam{4, false},
                                           InvalidPlayerCountTestParam{5, true} // More than maximum
                                           ));

struct InvalidVictoryCardCountTestParam
{
    size_t player_count;
    size_t victory_card_count;
    bool expect_game_over;
};

class BoardInvalidVictoryCardCountTest : public ::testing::TestWithParam<InvalidVictoryCardCountTestParam>
{
protected:
    void SetUp() override
    {
        kingdom_cards = get_valid_kingdom_cards();
        const auto &param = GetParam();
        player_count = param.player_count;
        board = new TestableSharedBoard(kingdom_cards, player_count);

        // Set invalid victory card counts
        const auto &victory_piles = board->getVictoryCards();
        for ( const auto &pile : victory_piles ) {
            if ( pile.card_id != "Curse" ) // Skip Curse cards
            {
                pile.count = param.victory_card_count;
            }
        }
    }

    void TearDown() override { delete board; }

    TestableSharedBoard *board;
    std::vector<shared::CardBase::id_t> kingdom_cards;
    size_t player_count;
};

TEST_P(BoardInvalidVictoryCardCountTest, IsGameOverWithInvalidVictoryCardCount)
{
    const auto &param = GetParam();
    EXPECT_EQ(board->isGameOver(), param.expect_game_over);
}

INSTANTIATE_TEST_SUITE_P(InvalidVictoryCardCountTests, BoardInvalidVictoryCardCountTest,
                         ::testing::Values(InvalidVictoryCardCountTestParam{2, 0, true}, // Victory cards depleted
                                           InvalidVictoryCardCountTestParam{2, 8, false}, // Valid count
                                           InvalidVictoryCardCountTestParam{3, 0, true}, // Victory cards depleted
                                           InvalidVictoryCardCountTestParam{3, 12, false} // Valid count
                                           ));
