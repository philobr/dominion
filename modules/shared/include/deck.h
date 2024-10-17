#pragma once

#include "types.h"
#include "cards.h"

#include <deque>
#include <vector>
#include <algorithm>

// forward declaration
class CardBase;

class Deck {
    std::deque<card_name_t> draw_pile; // easy push front and push back
    std::vector<card_name_t> discard_pile; // can be shuffeled

    std::vector<card_name_t> hand; // random access
    std::vector<card_name_t> played; // random access

public:
    Deck(); // initialize randomly, shuffle, then push to draw_pile

    void initialise_turn(); // discard played cards, redraw 5, if necessary shuffle
    void end_turn(); // discard played cards, redraw 5, if necessary shuffle

    void shuffle();
    void draw(int n);
    void maybe_draw(int n); // ?

    card_name_t peek(int n);

    void discard(card_type_t card);
    void remove(card_type_t card); // maybe trash? base game does not provide other ways to remove cards
    void add(card_type_t card);

    int get_points();
private:

};