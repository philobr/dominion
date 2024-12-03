
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
            shared::ChooseFrom::AllowedChoice allowed_choices;

            GET_UINT_MEMBER(min_cards, json, "min_cards");
            GET_UINT_MEMBER(max_cards, json, "max_cards");
            GET_ENUM_MEMBER(allowed_choices, json, "allowed_choices", shared::ChooseFrom::AllowedChoice);

            if ( type == "choose_from_hand" ) {
                return std::make_unique<ChooseFromHand>(min_cards, max_cards, allowed_choices);
            } else if ( type == "choose_from_staged" ) {
                std::vector<shared::CardBase::id_t> cards;
                GET_STRING_ARRAY_MEMBER(cards, json, "cards");
                return std::make_unique<ChooseFromStaged>(min_cards, max_cards, allowed_choices, cards);
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
        } else if ( typeid(*this) == typeid(ChooseFromHand) ) {
            doc.AddMember("type", "choose_from_hand", doc.GetAllocator());
            ADD_UINT_MEMBER(dynamic_cast<const ChooseFromHand *>(this)->min_cards, min_cards);
            ADD_UINT_MEMBER(dynamic_cast<const ChooseFromHand *>(this)->max_cards, max_cards);
            ADD_ENUM_MEMBER(dynamic_cast<const ChooseFromHand *>(this)->allowed_choices, allowed_choices);
        } else if ( typeid(*this) == typeid(ChooseFromStaged) ) {
            doc.AddMember("type", "choose_from_staged", doc.GetAllocator());
            ADD_UINT_MEMBER(dynamic_cast<const ChooseFromStaged *>(this)->min_cards, min_cards);
            ADD_UINT_MEMBER(dynamic_cast<const ChooseFromStaged *>(this)->max_cards, max_cards);
            ADD_ENUM_MEMBER(dynamic_cast<const ChooseFromStaged *>(this)->allowed_choices, allowed_choices);
            ADD_ARRAY_OF_STRINGS_MEMBER(dynamic_cast<const ChooseFromStaged *>(this)->cards, cards);
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

    bool ChooseFrom::operator==(const ChooseFrom &other) const
    {
        return min_cards == other.min_cards && max_cards == other.max_cards && allowed_choices == other.allowed_choices;
    }

    bool ChooseFrom::operator!=(const ChooseFrom &other) const { return !ChooseFrom::operator==(other); }

    bool ChooseFrom::equals(const ActionOrder &other) const { return *this == dynamic_cast<const ChooseFrom &>(other); }


    bool ChooseFromStaged::operator==(const ChooseFromStaged &other) const
    {
        return (cards.size() == other.cards.size() && std::equal(cards.begin(), cards.end(), other.cards.begin())) &&
                ChooseFrom::operator==(other);
    }

    bool ChooseFromStaged::operator!=(const ChooseFromStaged &other) const
    {
        return !ChooseFromStaged::operator==(other);
    }

    bool ChooseFromStaged::equals(const ActionOrder &other) const
    {
        return *this == dynamic_cast<const ChooseFromStaged &>(other);
    }


    bool ChooseFromHand::operator==(const ChooseFromHand &other) const { return !ChooseFrom::operator==(other); }

    bool ChooseFromHand::operator!=(const ChooseFromHand &other) const { return !ChooseFromHand::operator==(other); }

    bool ChooseFromHand::equals(const ActionOrder &other) const
    {
        return *this == dynamic_cast<const ChooseFromHand &>(other);
    }

} // namespace shared
