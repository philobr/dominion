
#include <shared/action_decision.h>
#include <typeinfo>

namespace shared
{
    bool ActionOrder::operator==(const ActionOrder &other) const
    {
        return typeid(*this) == typeid(other) && equals(other);
    }

    bool ActionOrder::operator!=(const ActionOrder &other) const { return !ActionOrder::operator==(other); }

    bool ActionPhaseOrder::operator==(const ActionPhaseOrder &other) const
    {
        return true;
    }

    bool ActionPhaseOrder::operator!=(const ActionPhaseOrder &other) const { return !ActionPhaseOrder::operator==(other); }

    bool ActionPhaseOrder::equals(const ActionOrder &other) const
    {
        return *this == dynamic_cast<const ActionPhaseOrder &>(other);
    }

    bool BuyPhaseOrder::operator==(const BuyPhaseOrder &other) const
    {
        return true;
    }

    bool BuyPhaseOrder::operator!=(const BuyPhaseOrder &other) const { return !BuyPhaseOrder::operator==(other); }

    bool BuyPhaseOrder::equals(const ActionOrder &other) const
    {
        return *this == dynamic_cast<const BuyPhaseOrder &>(other);
    }

    bool ChooseNCardsFromHandOrder::operator==(const ChooseNCardsFromHandOrder &other) const
    {
        return n == other.n;
    }

    bool ChooseNCardsFromHandOrder::operator!=(const ChooseNCardsFromHandOrder &other) const { return !ChooseNCardsFromHandOrder::operator==(other); }

    bool ChooseNCardsFromHandOrder::equals(const ActionOrder &other) const
    {
        return *this == dynamic_cast<const ChooseNCardsFromHandOrder &>(other);
    }
} // namespace shared
