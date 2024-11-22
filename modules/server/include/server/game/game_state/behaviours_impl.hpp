#include <server/game/game_state/behaviour.h>

namespace server
{
    namespace behaviour
    {

#define DEFINE_BEHAVIOUR(name, types)                                                                                  \
    class name : public BehaviourBase                                                                                  \
    {                                                                                                                  \
    public:                                                                                                            \
        inline ret_t                                                                                                   \
        apply(server::GameState *state,                                                                                \
              std::optional<std::unique_ptr<shared::ActionDecision>> action_decision = std::nullopt) const;            \
    };                                                                                                                 \
    inline BehaviourBase::ret_t name::apply(server::GameState *game_state,                                             \
                                            std::optional<std::unique_ptr<shared::ActionDecision>> action_decision)    \
            const

#define DEFINE_TEMPLATED_BEHAVIOUR(name, types, template_type, template_name)                                          \
    template <template_type template_name>                                                                             \
    class name : public BehaviourBase                                                                                  \
    {                                                                                                                  \
    public:                                                                                                            \
        inline ret_t                                                                                                   \
        apply(server::GameState *state,                                                                                \
              std::optional<std::unique_ptr<shared::ActionDecision>> action_decision = std::nullopt) const;            \
    };                                                                                                                 \
    template <template_type template_name>                                                                             \
    inline BehaviourBase::ret_t name<template_name>::apply(                                                            \
            server::GameState *game_state, std::optional<std::unique_ptr<shared::ActionDecision>> action_decision)     \
            const

        // ================================
        // BEHAVIOUR IMPLEMENTATIONS
        // ================================
        DEFINE_TEMPLATED_BEHAVIOUR(GainCoins, BehaviourType::NON_INTERACTIVE, int, coins)
        {
            CHECK_HAS_NO_DECISION;
            LOG(DEBUG) << "Applied GainCoins";

            auto &affected_player = game_state->get_current_player();
            affected_player.incTreasure(coins);
            return std::nullopt;
        }

        DEFINE_TEMPLATED_BEHAVIOUR(GainBuys, BehaviourType::NON_INTERACTIVE, int, buys)
        {
            CHECK_HAS_NO_DECISION;
            LOG(INFO) << "Applied GainCoins";

            auto &affected_player = game_state->get_current_player();
            affected_player.incBuys(buys);
            return std::nullopt;
        }

        DEFINE_TEMPLATED_BEHAVIOUR(GainActions, BehaviourType::NON_INTERACTIVE, int, actions)
        {
            CHECK_HAS_NO_DECISION;
            LOG(INFO) << "Applied GainCoins";

            auto &affected_player = game_state->get_current_player();
            affected_player.incActions(actions);
            return std::nullopt;
        }

        DEFINE_TEMPLATED_BEHAVIOUR(GainPoints, BehaviourType::NON_INTERACTIVE, int, points)
        {
            CHECK_HAS_NO_DECISION;
            LOG(INFO) << "Applied GainPoints";

            auto &affected_player = game_state->get_current_player();
            affected_player.incPoints(points);
            return std::nullopt;
        }

#define TODO_IMPLEMENT_ME                                                                                              \
    if ( game_state->is_game_over() || action_decision.has_value() ) {                                                 \
        LOG(ERROR) << "making the linter happy by using all the variables :D :D :D";                                   \
    }                                                                                                                  \
    LOG(ERROR) << "BEHAVIOUR IS NOT IMPLEMENTED YET";                                                                  \
    throw std::runtime_error("not implemented");                                                                       \
                                                                                                                       \
    return std::nullopt;

        // ================================
        // PLACEHOLDER BEHAVIOUR
        // ================================
        DEFINE_BEHAVIOUR(NOT_IMPLEMENTED_YET, BehaviourType::NON_INTERACTIVE) { TODO_IMPLEMENT_ME; }

        // ================================
        // TODO
        // ================================
        DEFINE_TEMPLATED_BEHAVIOUR(DrawCards, BehaviourType::NON_INTERACTIVE, int, cards) { TODO_IMPLEMENT_ME; }

        DEFINE_BEHAVIOUR(GainAnyCard, BehaviourType::NON_INTERACTIVE) { TODO_IMPLEMENT_ME; }

        // ================================
        // TODO: ADD MORE
        // ================================

#undef DEFINE_BEHAVIOUR
#undef DEFINE_TEMPLATED_BEHAVIOUR
    } // namespace behaviour
} // namespace server