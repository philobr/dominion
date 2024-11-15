#include <algorithm>
#include <shared/game/game_state/board_base.h>

namespace shared
{
    Board::Board(const std::vector<shared::CardBase::id_t> &kingdom_cards, size_t player_count)
    {
        _ASSERT_EQ(kingdom_cards.size(), BoardConfig::KINGDOM_CARD_COUNT,
                   "Board must be initialised with 10 kingdom cards!");
        _ASSERT_TRUE((BoardConfig::MIN_PLAYER_COUNT <= player_count && player_count <= BoardConfig::MAX_PLAYER_COUNT),
                     "Players must be in [2, 4]");

        std::transform(kingdom_cards.begin(), kingdom_cards.end(),
                       std::inserter(this->kingdom_cards, this->kingdom_cards.end()),
                       [](const shared::CardBase::id_t &card_id) { return Pile::makeKingdomCard(card_id); });

        initialiseTreasureCards(player_count);
        initialiseVictoryCards(player_count);
    }

    Board::ptr_t Board::make(const std::vector<shared::CardBase::id_t> &kingdom_cards, size_t player_count)
    {
        return ptr_t(new Board(kingdom_cards, player_count));
    }

    size_t Board::getEmptyPilesCount() const
    {
        auto count_empty = [](const auto &pile_set) -> size_t
        { return std::count_if(pile_set.begin(), pile_set.end(), [](const auto &pile) { return pile.count == 0; }); };

        return count_empty(treasure_cards) + count_empty(victory_cards) + count_empty(kingdom_cards);
    }

    bool Board::isGameOver() const
    {
        static constexpr size_t MAX_NUM_EMPTY_PILES = 3;
        auto province_pile = victory_cards.find("Province");
        return ((province_pile != victory_cards.end()) && (province_pile->count == 0)) ||
                (getEmptyPilesCount() >= MAX_NUM_EMPTY_PILES);
    }

    void Board::initialiseTreasureCards(size_t player_count)
    {
        const size_t copper_count = BoardConfig::TREASURE_COPPER_COUNT - (7 * player_count);
        treasure_cards = {Pile::make("Copper", copper_count), Pile::make("Silver", BoardConfig::TREASURE_SILVER_COUNT),
                          Pile::make("Gold", BoardConfig::TREASURE_GOLD_COUNT)};
    }

    void Board::initialiseVictoryCards(size_t player_count)
    {
        const size_t card_count =
                (player_count < 3) ? BoardConfig::VICTORY_CARDS_SMALL_GAME : BoardConfig::VICTORY_CARDS_LARGE_GAME;
        const size_t curse_count = (player_count - 1) * BoardConfig::CURSE_MULTIPLIER;

        victory_cards = {Pile::make("Estate", card_count), Pile::make("Duchy", card_count),
                         Pile::make("Province", card_count), Pile::make("Curse", curse_count)};
    }

} // namespace shared
