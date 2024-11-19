
#pragma once

#include <memory>
#include <rapidjson/document.h>

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
        static std::unique_ptr<ActionOrder> from_json(const rapidjson::Value &json);
        /**
         * @brief Convert this order to a JSON object.
         */
        rapidjson::Document to_json() const;

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
} // namespace shared
