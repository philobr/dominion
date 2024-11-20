
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
        return this->cardIndex == other.cardIndex;
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

    bool EndTurnDecision::operator==(const EndTurnDecision & /*other*/) const { return true; }

    bool EndTurnDecision::equals(const ActionDecision &other) const
    {
        return *this == dynamic_cast<const EndTurnDecision &>(other);
    }

    bool ChooseNCardsFromHandDecision::operator==(const ChooseNCardsFromHandDecision &other) const
    {
        return this->cards == other.cards;
    }

    bool ChooseNCardsFromHandDecision::equals(const ActionDecision &other) const
    {
        return *this == dynamic_cast<const ChooseNCardsFromHandDecision &>(other);
    }
} // namespace shared
