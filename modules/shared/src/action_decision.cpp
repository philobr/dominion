
#include <shared/action_decision.h>
#include <typeinfo>

namespace shared
{
    bool ActionDecision::operator==(const ActionDecision &other) const
    {
        return typeid(*this) == typeid(other) && equals(other);
    }

    bool ActionDecision::operator!=(const ActionDecision &other) const { return !ActionDecision::operator==(other); }

    bool PlayActionCardDecision::operator==(const PlayActionCardDecision &other) const
    {
        return (this->card_id == other.card_id) && (this->from == other.from);
    }

    bool PlayActionCardDecision::equals(const ActionDecision &other) const
    {
        return *this == dynamic_cast<const PlayActionCardDecision &>(other);
    }

    bool BuyCardDecision::operator==(const BuyCardDecision &other) const { return this->card == other.card; }

    bool BuyCardDecision::equals(const ActionDecision &other) const
    {
        return *this == dynamic_cast<const BuyCardDecision &>(other);
    }

    bool EndActionPhaseDecision::operator==(const EndActionPhaseDecision & /*other*/) const { return true; }

    bool EndActionPhaseDecision::equals(const ActionDecision &other) const
    {
        return *this == dynamic_cast<const EndActionPhaseDecision &>(other);
    }

    bool EndTurnDecision::operator==(const EndTurnDecision & /*other*/) const { return true; }

    bool EndTurnDecision::equals(const ActionDecision &other) const
    {
        return *this == dynamic_cast<const EndTurnDecision &>(other);
    }


    bool DeckChoiceDecision::operator==(const DeckChoiceDecision &other) const
    {
        return (cards.size() == other.cards.size() && choices.size() == other.choices.size()) &&
                std::equal(cards.begin(), cards.end(), other.cards.begin()) &&
                std::equal(choices.begin(), choices.end(), other.choices.begin());
    }

    bool DeckChoiceDecision::operator!=(const DeckChoiceDecision &other) const
    {
        return DeckChoiceDecision::operator==(other);
    }

    bool DeckChoiceDecision::equals(const ActionDecision &other) const
    {
        return *this == dynamic_cast<const DeckChoiceDecision &>(other);
    }

    bool BoardChoiceDecision::operator==(const BoardChoiceDecision &other) const
    {
        return chosen_card == other.chosen_card;
    }

    bool BoardChoiceDecision::operator!=(const BoardChoiceDecision &other) const
    {
        return BoardChoiceDecision::operator==(other);
    }

    bool BoardChoiceDecision::equals(const ActionDecision &other) const
    {
        return *this == dynamic_cast<const BoardChoiceDecision &>(other);
    }
} // namespace shared
