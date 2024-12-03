
#include <shared/action_order.h>
#include <shared/utils/json.h>
#include <string>
#include <typeinfo>

namespace shared
{
    bool ActionOrder::operator==(const ActionOrder &other) const
    {
        return typeid(*this) == typeid(other) && equals(other);
    }

    bool ActionOrder::operator!=(const ActionOrder &other) const { return !ActionOrder::operator==(other); }

    std::unique_ptr<ActionOrder> ActionOrder::fromJson(const rapidjson::Value &json)
    {
        std::string type;
        GET_STRING_MEMBER(type, json, "type");
        if ( type == "action_phase" ) {
            return std::make_unique<ActionPhaseOrder>();
        } else if ( type == "buy_phase" ) {
            return std::make_unique<BuyPhaseOrder>();
        } else if ( type == "gain_card" ) {
            unsigned int max_cost;
            shared::CardType allowed_type;
            return std::make_unique<GainFromBoard>(max_cost, allowed_type);
        } else if ( type == "choose_from_hand" || type == "choose_from_staged" ) {
            unsigned int min_cards;
            unsigned int max_cards;
            shared::ChooseFromOrder::AllowedChoice allowed_choices;

            GET_UINT_MEMBER(min_cards, json, "min_cards");
            GET_UINT_MEMBER(max_cards, json, "max_cards");
            GET_ENUM_MEMBER(allowed_choices, json, "allowed_choices", shared::ChooseFromOrder::AllowedChoice);

            if ( type == "choose_from_hand" ) {
                return std::make_unique<ChooseFromHandOrder>(min_cards, max_cards, allowed_choices);
            } else if ( type == "choose_from_staged" ) {
                std::vector<shared::CardBase::id_t> cards;
                GET_STRING_ARRAY_MEMBER(cards, json, "cards");
                return std::make_unique<ChooseFromStagedOrder>(min_cards, max_cards, allowed_choices, cards);
            } else {
                return nullptr;
            }
        } else {
            return nullptr;
        }
    }

    rapidjson::Document ActionOrder::toJson() const
    {
        rapidjson::Document doc;
        doc.SetObject();
        if ( typeid(*this) == typeid(ActionPhaseOrder) ) {
            doc.AddMember("type", "action_phase", doc.GetAllocator());
        } else if ( typeid(*this) == typeid(BuyPhaseOrder) ) {
            doc.AddMember("type", "buy_phase", doc.GetAllocator());
        } else if ( typeid(*this) == typeid(GainFromBoard) ) {
            doc.AddMember("type", "gain_card", doc.GetAllocator());
            ADD_UINT_MEMBER(dynamic_cast<const GainFromBoard *>(this)->max_cost, max_cost);
            ADD_ENUM_MEMBER(dynamic_cast<const GainFromBoard *>(this)->allowed_type, allowed_type);
        } else if ( typeid(*this) == typeid(ChooseFromHandOrder) ) {
            doc.AddMember("type", "choose_from_hand", doc.GetAllocator());
            ADD_UINT_MEMBER(dynamic_cast<const ChooseFromHandOrder *>(this)->min_cards, min_cards);
            ADD_UINT_MEMBER(dynamic_cast<const ChooseFromHandOrder *>(this)->max_cards, max_cards);
            ADD_ENUM_MEMBER(dynamic_cast<const ChooseFromHandOrder *>(this)->allowed_choices, allowed_choices);
        } else if ( typeid(*this) == typeid(ChooseFromStagedOrder) ) {
            doc.AddMember("type", "choose_from_staged", doc.GetAllocator());
            ADD_UINT_MEMBER(dynamic_cast<const ChooseFromStagedOrder *>(this)->min_cards, min_cards);
            ADD_UINT_MEMBER(dynamic_cast<const ChooseFromStagedOrder *>(this)->max_cards, max_cards);
            ADD_ENUM_MEMBER(dynamic_cast<const ChooseFromStagedOrder *>(this)->allowed_choices, allowed_choices);
            ADD_ARRAY_OF_STRINGS_MEMBER(dynamic_cast<const ChooseFromStagedOrder *>(this)->cards, cards);
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

    bool EndTurnOrder::operator==(const EndTurnOrder &other) const { return this->equals(other); }

    bool EndTurnOrder::operator!=(const EndTurnOrder &other) const { return !EndTurnOrder::operator==(other); }

    bool EndTurnOrder::equals(const ActionOrder &other) const
    {
        return *this == dynamic_cast<const EndTurnOrder &>(other);
    }

    bool GainFromBoard::operator==(const GainFromBoard &other) const
    {
        return max_cost == other.max_cost && allowed_type == other.allowed_type;
    }

    bool GainFromBoard::equals(const ActionOrder &other) const
    {
        return *this == dynamic_cast<const GainFromBoard &>(other);
    }

    bool ChooseFromOrder::operator==(const ChooseFromOrder &other) const
    {
        return min_cards == other.min_cards && max_cards == other.max_cards && allowed_choices == other.allowed_choices;
    }

    bool ChooseFromOrder::operator!=(const ChooseFromOrder &other) const { return !ChooseFromOrder::operator==(other); }

    bool ChooseFromOrder::equals(const ActionOrder &other) const
    {
        return *this == dynamic_cast<const ChooseFromOrder &>(other);
    }


    bool ChooseFromStagedOrder::operator==(const ChooseFromStagedOrder &other) const
    {
        return (cards.size() == other.cards.size() && std::equal(cards.begin(), cards.end(), other.cards.begin())) &&
                ChooseFromOrder::operator==(other);
    }

    bool ChooseFromStagedOrder::operator!=(const ChooseFromStagedOrder &other) const
    {
        return !ChooseFromStagedOrder::operator==(other);
    }

    bool ChooseFromStagedOrder::equals(const ActionOrder &other) const
    {
        return *this == dynamic_cast<const ChooseFromStagedOrder &>(other);
    }


    bool ChooseFromHandOrder::operator==(const ChooseFromHandOrder &other) const
    {
        return !ChooseFromOrder::operator==(other);
    }

    bool ChooseFromHandOrder::operator!=(const ChooseFromHandOrder &other) const
    {
        return !ChooseFromHandOrder::operator==(other);
    }

    bool ChooseFromHandOrder::equals(const ActionOrder &other) const
    {
        return *this == dynamic_cast<const ChooseFromHandOrder &>(other);
    }

} // namespace shared
