#pragma once

#include <string>

#include "types.h"
#include "card_factory.h"

// forward declaration
class Player;

class CardBase {
    const int cost_m;
    const card_name_t id_m;
    const card_type_t card_type_m;

public:
    CardBase(int cost, const card_name_t& id, const card_type_t& card_type)
        : cost_m(cost), id_m(id), card_type_m(card_type)
    { }
    // test
    virtual ~CardBase() = default;

    virtual card_name_t getCardType() const { return card_type_m; }
    card_name_t getId() const { return id_m; }
    int getCost() const { return cost_m; }

    // implement to string now, can then maybe be used for json
    // make virtual, inherited cards can then return: inherited_card::to_string() const { return CardBase().to_string() + "this string"; }
    std::string to_string() const { return "needs to be implemented in basecard first!"; }
};