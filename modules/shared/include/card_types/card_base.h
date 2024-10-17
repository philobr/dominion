#pragma once

#include <string>

#include "types.h"
#include "card_factory.h"

class Player;

class CardBase {
    const int cost_m;
    const card_id_t id_m;
    const card_type_t card_type_m;

public:
    CardBase(int cost, const card_id_t& id, const card_type_t& card_type)
        : cost_m(cost), id_m(id), card_type_m(card_type)
    { }

    virtual ~CardBase() = default;

    virtual card_id_t getCardType() const { return card_type_m; }
    card_id_t getId() const { return id_m; }
    int getCost() const { return cost_m; }
};