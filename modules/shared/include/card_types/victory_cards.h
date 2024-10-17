#pragma once

#include "card_base.h"

class VictoryCard : public CardBase {
    const int points_m;
public:
    VictoryCard(int cost, const card_id_t& id, int points) : CardBase(cost, id, "Victory"), points_m(points) { }
    int getPoints() const { return points_m; }
};

#define REGISTER_VICTORY_CARD(id, cost, points)                    \
    class id : public VictoryCard {                                \
    public:                                                        \
        id() : VictoryCard(cost, #id, points) { }                  \
    };                                                             \
    static bool id##_registered = (CardFactory::registerCard(#id, std::make_unique<id>()), true);

REGISTER_VICTORY_CARD(Estate, 2, 1);
REGISTER_VICTORY_CARD(Duchy, 5, 3);
REGISTER_VICTORY_CARD(Province, 8, 6);