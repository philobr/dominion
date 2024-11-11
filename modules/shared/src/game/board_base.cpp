#include <algorithm>
#include <shared/game/game_state/board_base.h>

namespace shared
{
    size_t Board::countEmptyPiles() const
    {
        auto count_empty = [](auto pile_vector) -> size_t
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
                           [](auto pile) { return pile.card == "province" && pile.count == 0; }) &&
                countEmptyPiles() < 3;
    }

} // namespace shared
