#pragma once

#include "types.h"
#include "cards.h"

class Board {
public:
    struct Pile {
        card_name_t card_name;
        int count;
    };

private:

    std::vector<Pile> kingdom_cards;
    std::vector<Pile> treasure_cards;
    std::vector<Pile> victory_cards; // including curses ig

    std::vector<card_name_t> trash;

    int empty_piles;

public:

    Board();

    static Board make_board(const std::vector<card_name_t>& cards);

    bool game_over() const; // count empty piles
    bool buy(card_name_t card); // here also player?
    void trash(card_name_t card);
    bool take(card_name_t card);

private:
};