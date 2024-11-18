
#pragma once

namespace shared
{
    class ActionOrder
    {
    public:
        virtual ~ActionOrder() = default;
        bool operator==(const ActionOrder &other) const;
        bool operator!=(const ActionOrder &other) const;

    protected:
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
