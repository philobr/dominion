#pragma once

#include <vector>

#include <shared/game/cards/card_base.h>
namespace shared
{
    struct Pile
    {
        shared::CardBase::id_t card;
        size_t count;
    };

    struct Board
    {
        std::vector<Pile> victory_cards;
        std::vector<Pile> treasure_cards;
        std::vector<Pile> kingdom_cards;
        std::vector<shared::CardBase::id_t> played_cards;
        std::vector<shared::CardBase::id_t> trash;

        bool isGameOver() const;
        size_t countEmptyPiles() const;
        void initialise_treasure_cards(size_t player_count);
        void initialise_victory_cards(size_t player_count);
    };

} // namespace shared
