
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

    class DeckChoiceDecision : public ActionDecision
    {
    public:
        enum AllowedChoice
        {
            PLAY,
            TRASH,
            DISCARD,
            DRAW_PILE
        };

        std::vector<shared::CardBase::id_t> cards;
        std::vector<AllowedChoice> choices;

        DeckChoiceDecision(std::vector<shared::CardBase::id_t> cards, std::vector<AllowedChoice> choices) :
            cards(cards), choices(choices)
        {}

        DeckChoiceDecision(shared::CardBase::id_t card, AllowedChoice choices) : cards(cards), choices(choices) {}

        bool operator==(const EndTurnDecision &other) const;
        bool operator!=(const EndTurnDecision &other) const;

    protected:
        bool equals(const ActionDecision &other) const override;
    };

    class BoardChoiceDecision : public ActionDecision
    {
    public:
        shared::CardBase::id_t chosen_card;
    };
} // namespace shared
