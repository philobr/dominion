#pragma once

#include "card_base.h"

class TreasureCard : public CardBase {
    const int value_m;
public:
    TreasureCard(int cost, const card_id_t& id, int value) : CardBase(cost, id, "Treasure"), value_m(value) { }

    int getValue() const { return value_m; }
};

#define REGISTER_TREASURE_CARD(id, cost, value)                     \
    class id : public TreasureCard {                                \
    public:                                                         \
        id() : TreasureCard(cost, #id, value) { }                   \
    };                                                              \
    static bool id##_registered = (CardFactory::registerCard(#id, std::make_unique<id>()), true);

REGISTER_TREASURE_CARD(Copper, 0, 1);
REGISTER_TREASURE_CARD(Silver, 3, 2);
REGISTER_TREASURE_CARD(Gold, 6, 3);
