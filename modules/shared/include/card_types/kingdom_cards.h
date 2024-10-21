#pragma once

#include "card_base.h"
#include "player.h"

// forward declaration
class Player;

class KingdomCard : public CardBase {
    const int coins_m;
    const int buys_m;
    const int cards_m;
    const int actions_m;

public:
    KingdomCard(int cost, const card_name_t& id, const card_type_t& card_type, int coins, int buys, int cards, int actions)
        : CardBase(cost, id, card_type),
        coins_m(coins), buys_m(buys), cards_m(cards), actions_m(actions)
    { }

    void extra_coins(Player* player) const { player->add_coins(coins_m); }
    void extra_buys(Player* player) const { player->add_buys(buys_m); }
    void extra_cards(Player* player) const { player->draw_cards(cards_m); }
    void extra_actions(Player* player) const { player->add_actions(actions_m); }
};

class TrivialAction : public KingdomCard {
public:
    TrivialAction(int cost, const card_name_t& id, int coins, int buys, int cards, int actions)
        : KingdomCard(cost, id, "Action", coins, buys, cards, actions)
    { }
};

#define REGISTER_TRIVIAL_KINGDOM_CARD(id, cost, coins, buys, cards, actions)    \
    class id : public TrivialAction {                                           \
    public:                                                                     \
        id() : TrivialAction(cost, #id, coins, buys, cards, actions) { }        \
    };                                                                          \
    static bool id##_registered = (CardFactory::registerCard(#id, std::make_unique<id>()), true);


REGISTER_TRIVIAL_KINGDOM_CARD(Village, 3, 0, 0, 1, 2);
REGISTER_TRIVIAL_KINGDOM_CARD(Smithy, 4, 0, 0, 3, 0);
REGISTER_TRIVIAL_KINGDOM_CARD(Festival, 5, 2, 1, 0, 2);
REGISTER_TRIVIAL_KINGDOM_CARD(Laboratory, 5, 0, 0, 2, 1);
REGISTER_TRIVIAL_KINGDOM_CARD(Market, 5, 1, 1, 1, 1);
