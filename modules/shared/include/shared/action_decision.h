
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
        PlayActionCardDecision(shared::CardBase::id_t card_id,
                               shared::CardAccess from_pile = shared::CardAccess::HAND) :
            card_id(card_id),
            from(from_pile)
        {}

        shared::CardBase::id_t card_id;
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

    class EndActionPhaseDecision : public ActionDecision
    {
    public:
        bool operator==(const EndActionPhaseDecision &other) const;
        bool operator!=(const EndActionPhaseDecision &other) const;
        EndActionPhaseDecision() = default;

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

    class DeckChoiceDecision : public ActionDecision
    {
    public:
        enum AllowedChoice
        {
            PLAY = 1,
            TRASH = 2,
            DISCARD = 4,
            DRAW_PILE = 8,
            HAND_CARDS = 16
        };

        std::vector<shared::CardBase::id_t> cards;
        std::vector<AllowedChoice> choices;

        DeckChoiceDecision(std::vector<shared::CardBase::id_t> cards, std::vector<AllowedChoice> choices) :
            cards(cards), choices(choices)
        {}

        bool operator==(const DeckChoiceDecision &other) const;
        bool operator!=(const DeckChoiceDecision &other) const;

    protected:
        bool equals(const ActionDecision &other) const override;
    };

    class GainFromBoardDecision : public ActionDecision
    {
    public:
        shared::CardBase::id_t chosen_card;
        GainFromBoardDecision(shared::CardBase::id_t chosen_card) : chosen_card(chosen_card) {}
        bool operator==(const GainFromBoardDecision &other) const;
        bool operator!=(const GainFromBoardDecision &other) const;

    protected:
        bool equals(const ActionDecision &other) const override;
    };
} // namespace shared
