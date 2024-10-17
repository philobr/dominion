#pragma once

#include "types.h"
#include "cards.h"
#include "deck.h"
#include <string>

// forward declaration
class CardBase;

class Player {
    player_id id; // will be set by the server maybe?
    const std::string name; // if none is set assign default name?
    Deck deck;

    int available_actions;
    int available_buys;
    int available_coins;

public:
    Player(const std::string& name)
        : name(name),
        available_actions(1), available_buys(0), available_coins(0)
    { }

    void initialise_turn();
    void end_turn();

    void add_actions(int n) { available_actions += n; }
    void add_buys(int n) { available_buys += n; }
    void add_coins(int n) { available_coins += n; }

    void draw_cards(int n);
    void discard_card(card_name_t card);
    void trash_card(card_name_t card);
    void buy_card(card_name_t card);

private:

};