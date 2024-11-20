
#include <shared/action_order.h>
#include <shared/utils/json.h>
#include <typeinfo>

namespace shared
{
    bool ActionOrder::operator==(const ActionOrder &other) const
    {
        return typeid(*this) == typeid(other) && equals(other);
    }

    bool ActionOrder::operator!=(const ActionOrder &other) const { return !ActionOrder::operator==(other); }

    std::unique_ptr<ActionOrder> ActionOrder::from_json(const rapidjson::Value &json)
    {
        std::string type;
        GET_STRING_MEMBER(type, json, "type");
        if ( type == "action_phase" ) {
            return std::make_unique<ActionPhaseOrder>();
        } else if ( type == "buy_phase" ) {
            return std::make_unique<BuyPhaseOrder>();
        } else if ( type == "choose_n_cards_from_hand" ) {
            unsigned int n;
            GET_UINT_MEMBER(n, json, "n");
            return std::make_unique<ChooseNCardsFromHandOrder>(n);
        } else {
            return nullptr;
        }
    }

    rapidjson::Document ActionOrder::to_json() const
    {
        rapidjson::Document doc;
        doc.SetObject();
        if ( typeid(*this) == typeid(ActionPhaseOrder) ) {
            doc.AddMember("type", "action_phase", doc.GetAllocator());
        } else if ( typeid(*this) == typeid(BuyPhaseOrder) ) {
            doc.AddMember("type", "buy_phase", doc.GetAllocator());
        } else if ( typeid(*this) == typeid(ChooseNCardsFromHandOrder) ) {
            doc.AddMember("type", "choose_n_cards_from_hand", doc.GetAllocator());
            doc.AddMember("n", dynamic_cast<const ChooseNCardsFromHandOrder *>(this)->n, doc.GetAllocator());
        }
        return doc;
    }

    bool ActionPhaseOrder::operator==(const ActionPhaseOrder & /* other */) const { return true; }

    bool ActionPhaseOrder::operator!=(const ActionPhaseOrder &other) const
    {
        return !ActionPhaseOrder::operator==(other);
    }

    bool ActionPhaseOrder::equals(const ActionOrder &other) const
    {
        return *this == dynamic_cast<const ActionPhaseOrder &>(other);
    }

    bool BuyPhaseOrder::operator==(const BuyPhaseOrder & /* other */) const { return true; }

    bool BuyPhaseOrder::operator!=(const BuyPhaseOrder &other) const { return !BuyPhaseOrder::operator==(other); }

    bool BuyPhaseOrder::equals(const ActionOrder &other) const
    {
        return *this == dynamic_cast<const BuyPhaseOrder &>(other);
    }

    bool ChooseNCardsFromHandOrder::operator==(const ChooseNCardsFromHandOrder &other) const { return n == other.n; }

    bool ChooseNCardsFromHandOrder::operator!=(const ChooseNCardsFromHandOrder &other) const
    {
        return !ChooseNCardsFromHandOrder::operator==(other);
    }

    bool ChooseNCardsFromHandOrder::equals(const ActionOrder &other) const
    {
        return *this == dynamic_cast<const ChooseNCardsFromHandOrder &>(other);
    }
} // namespace shared
