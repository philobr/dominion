
#pragma once

#include <memory>
#include <rapidjson/document.h>
#include <shared/game/cards/card_base.h>
#include <shared/game/game_state/player_base.h>

namespace shared
{
    /**
     * @brief A request to a player to perform an action.
     *
     * An action order is a request to a player to perform an action. The player
     * must respond with an `ActionDecision` that satisfies the order.
     *
     * The order is a polymorphic type that can represent different types of
     * phases of the game.
     */
    class ActionOrder
    {
    public:
        virtual ~ActionOrder() = default;
        /**
         * @brief Check if this order is equal to another order.
         *
         * Two orders are equal if they are of the same type and have the same
         * values.
         *
         * @param other The other order to compare to.
         * @return True if the orders are equal, false otherwise.
         */
        bool operator==(const ActionOrder &other) const;
        /**
         * @brief Check if this order is not equal to another order.
         *
         * @see operator==
         */
        bool operator!=(const ActionOrder &other) const;

        /**
         * @brief Create an `ActionOrder` from a JSON object.
         */
        static std::unique_ptr<ActionOrder> fromJson(const rapidjson::Value &json);
        /**
         * @brief Convert this order to a JSON object.
         */
        rapidjson::Document toJson() const;

    protected:
        /**
         * @brief Virtual function to check if this order is equal to another order.
         *
         * Any derived class must implement this function to compare itself to
         * another order of the same type.
         * This function is called by the `operator==` function.
         * The `operator==` function checks if the orders are of the same type
         * and then calls this function to check if the fields of the orders are equal.
         *
         * @param other The other order to compare to. Must be of the same type. Otherwise, the result is undefined.
         * @return True if the orders are equal, false otherwise.
         */
        virtual bool equals(const ActionOrder &other) const = 0;
    };

    class ActionPhaseOrder : public ActionOrder
    {
    public:
        ActionPhaseOrder() = default;
        bool operator==(const ActionPhaseOrder &other) const;
        bool operator!=(const ActionPhaseOrder &other) const;

    protected:
        bool equals(const ActionOrder &other) const override;
    };

    class BuyPhaseOrder : public ActionOrder
    {
    public:
        BuyPhaseOrder() = default;
        bool operator==(const BuyPhaseOrder &other) const;
        bool operator!=(const BuyPhaseOrder &other) const;

    protected:
        bool equals(const ActionOrder &other) const override;
    };

    class ChooseNCardsFromHandOrder : public ActionOrder
    {
    public:
        ChooseNCardsFromHandOrder(unsigned int n) : n(n) {}
        bool operator==(const ChooseNCardsFromHandOrder &other) const;
        bool operator!=(const ChooseNCardsFromHandOrder &other) const;
        unsigned int n;

    protected:
        bool equals(const ActionOrder &other) const override;
    };

    class EndTurnOrder : public ActionOrder
    {
    public:
        EndTurnOrder() = default;
        bool operator==(const EndTurnOrder &other) const;
        bool operator!=(const EndTurnOrder &other) const;

    protected:
        bool equals(const ActionOrder &other) const override;
    };

    class GainFromBoard : public ActionOrder
    {
    public:
        GainFromBoard(unsigned int max_cost, shared::CardType allowed_type) :
            max_cost(max_cost), allowed_type(allowed_type)
        {}
        bool operator==(const GainFromBoard &other) const;
        bool operator!=(const GainFromBoard &other) const;

        unsigned int max_cost;
        shared::CardType allowed_type;

    protected:
        bool equals(const ActionOrder &other) const override;
    };

    class ChooseFrom : public ActionOrder
    {
    public:
        enum AllowedChoice
        {
            PLAY,
            TRASH,
            DISCARD,
            DRAW_PILE
        };

        ChooseFrom(unsigned int max_cards, AllowedChoice allowed_choices) :
            max_cards(max_cards), allowed_choices(allowed_choices)
        {}
        bool operator==(const ChooseFrom &other) const;
        bool operator!=(const ChooseFrom &other) const;

        AllowedChoice allowed_choices;
        unsigned int max_cards;

    protected:
        bool equals(const ActionOrder &other) const override;
    };

    class ChooseFromStaged : public ChooseFrom
    {
    public:
        ChooseFromStaged(std::vector<shared::CardBase::id_t> cards, AllowedChoice choices) :
            cards(cards), ChooseFrom(cards.size(), choices)
        {}

        bool operator==(const ChooseFromStaged &other) const;
        bool operator!=(const ChooseFromStaged &other) const;

        std::vector<shared::CardBase::id_t> cards;

    protected:
        bool equals(const ActionOrder &other) const override;
    };

    class ChooseFromHand : public ChooseFrom
    {
    public:
        ChooseFromHand(unsigned int max_cards, AllowedChoice choices) : ChooseFrom(max_cards, choices) {}

        bool operator==(const ChooseFromHand &other) const;
        bool operator!=(const ChooseFromHand &other) const;

    protected:
        bool equals(const ActionOrder &other) const override;
    };
} // namespace shared
