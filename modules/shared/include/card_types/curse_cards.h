#pragma once

#include "card_base.h"

class Curse : public CardBase {
    const int points_m;
public:
    Curse() : CardBase(0, "Curse", "Curse"), points_m(-1) { }
    int getPoints() const { return points_m; }
};
static bool idCurse_registered = (CardFactory::registerCard("Curse", std::make_unique<Curse>()), true);
