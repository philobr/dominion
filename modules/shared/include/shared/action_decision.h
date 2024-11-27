
#pragma once

#include <vector>

#include <shared/game/cards/card_base.h>
#include <shared/game/game_state/player_base.h>
namespace shared
{
    class ActionDecision
    {
    public:
        virtual ~ActionDecision() = default;
        bool operator==(const ActionDecision &other) const;
        bool operator!=(const ActionDecision &other) const;

    protected:
        virtual bool equals(const ActionDecision &other) const = 0;
    };

    class PlayActionCardDecision : public ActionDecision
    {
    public:
        bool operator==(const PlayActionCardDecision &other) const;
        bool operator!=(const PlayActionCardDecision &other) const;
        PlayActionCardDecision(unsigned int cardIndex, shared::CardAccess fromPile = shared::CardAccess::HAND) :
            cardIndex(cardIndex), from(fromPile)
        {}
        unsigned int cardIndex;
        shared::CardAccess from;

    protected:
        bool equals(const ActionDecision &other) const override;
    };

    class BuyCardDecision : public ActionDecision
    {
    public:
        bool operator==(const BuyCardDecision &other) const;
        bool operator!=(const BuyCardDecision &other) const;
        BuyCardDecision(CardBase::id_t card) : card(card) {}
        CardBase::id_t card;

    protected:
        bool equals(const ActionDecision &other) const override;
    };

    class EndTurnDecision : public ActionDecision
    {
    public:
        bool operator==(const EndTurnDecision &other) const;
        bool operator!=(const EndTurnDecision &other) const;
        EndTurnDecision() = default;

    protected:
        bool equals(const ActionDecision &other) const override;
    };

    class ChooseNCardsFromHandDecision : public ActionDecision
    {
    public:
        bool operator==(const ChooseNCardsFromHandDecision &other) const;
        bool operator!=(const ChooseNCardsFromHandDecision &other) const;
        ChooseNCardsFromHandDecision(std::vector<unsigned int> cards) : cards(cards) {}
        // TODO: Where do we enforce that these indices are unique?
        // why dont we just use CardBase::id_t here? would make life easier in game_state
        std::vector<unsigned int> cards;

    protected:
        bool equals(const ActionDecision &other) const override;
    };
} // namespace shared
