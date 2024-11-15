#include <algorithm>
#include <shared/game/game_state/board_base.h>

namespace shared
{
    Board::Board(const std::vector<shared::CardBase::id_t> &kingdom_cards, size_t player_count)
    {
        _ASSERT_EQ(kingdom_cards.size(), 10, "Board must be initialised with 10 kingdom cards!");
        _ASSERT_TRUE((2 <= player_count && player_count <= 4), "Players must be in [2, 4]");

        std::transform(kingdom_cards.begin(), kingdom_cards.end(),
                       std::inserter(this->kingdom_cards, this->kingdom_cards.end()),
                       [](const shared::CardBase::id_t &card_id) { return Pile{card_id, INITIAL_NUM_KINGDOM_CARDS}; });

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
        auto province_pile = victory_cards.find("Province");
        return ((province_pile != victory_cards.end()) && (province_pile->count == 0)) || (getEmptyPilesCount() >= 3);
    }

    void Board::initialiseTreasureCards(size_t player_count)
    {
        const size_t copper_count = 60 - (7 * player_count);
        const size_t silver_count = 40;
        const size_t gold_count = 30;

        treasure_cards = {{"Copper", copper_count}, {"Silver", silver_count}, {"Gold", gold_count}};
    }

    void Board::initialiseVictoryCards(size_t player_count)
    {
        const size_t card_count = (player_count < 3) ? 8 : 12;
        const size_t curse_count = (player_count - 1) * 10;
        victory_cards = {
                {"Estate", card_count}, {"Duchy", card_count}, {"Province", card_count}, {"Curse", curse_count}};
    }

} // namespace shared
