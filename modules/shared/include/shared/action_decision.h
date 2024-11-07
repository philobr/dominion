
#pragma once

#include "game_state.h"

namespace shared
{
    class ActionDecision
    {
    public:
        // Make this polymorphic
        virtual ~ActionDecision() {}
    };

    class PlayActionCardDecision : public ActionDecision
    {
    public:
        PlayActionCardDecision(unsigned int cardIndex) : cardIndex(cardIndex) {}
        unsigned int cardIndex;
    };

    class BuyCardDecision : public ActionDecision
    {
    public:
        BuyCardDecision(CardBase::id_t card) : card(card) {}
        CardBase::id_t card;
    };

    class EndTurnDecision : public ActionDecision
    {
    public:
        EndTurnDecision() {}
    };

    class ChooseNCardsFromHandDecision : public ActionDecision
    {
    public:
        ChooseNCardsFromHandDecision(std::vector<unsigned int> cards) : cards(cards) {}
        // TODO: Where do we enforce that these indices are unique?
        // tricky, as the same card type can be in the hand multiple times
        std::vector<unsigned int> cards;
    };
} // namespace shared
