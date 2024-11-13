#include <algorithm>
#include <shared/game/game_state/board_base.h>

namespace shared
{
    size_t Board::countEmptyPiles() const
    {
        auto count_empty = [](const auto &pile_vector) -> size_t
        {
            return std::count_if(pile_vector.begin(), pile_vector.end(),
                                 [](const auto &pile) { return pile.count == 0; });
        };

        return count_empty(treasure_cards) + count_empty(victory_cards) + count_empty(kingdom_cards);
    }

    bool Board::isGameOver() const
    {
        // any_of is recommended by linter, dont blame me
        return std::any_of(victory_cards.begin(), victory_cards.end(),
                           [](auto pile) { return pile.card == "Province" && pile.count == 0; }) ||
                countEmptyPiles() >= 3;
    }

    void Board::initialise_treasure_cards(size_t player_count)
    {
        const size_t copper_count = 60 - (7 * player_count);
        const size_t silver_count = 40;
        const size_t gold_count = 30;

        treasure_cards = {{"Copper", copper_count}, {"Silver", silver_count}, {"Gold", gold_count}};
    }

    void Board::initialise_victory_cards(size_t player_count)
    {
        const size_t card_count = (player_count < 3) ? 8 : 12;
        const size_t curse_count = (player_count - 1) * 10;
        victory_cards = {
                {"Estate", card_count}, {"Duchy", card_count}, {"Province", card_count}, {"Curse", curse_count}};
    }

} // namespace shared
