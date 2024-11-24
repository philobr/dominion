#include <server/game/game_state/behaviour_base.h>
#include <shared/utils/utils.h>
namespace server
{
    namespace behaviour
    {
        // TODO: define a stateful behaviour, ask @aaron if you need this

#define DEFINE_BEHAVIOUR(name, types)                                                                                  \
    class name : public BehaviourBase                                                                                  \
    {                                                                                                                  \
    public:                                                                                                            \
        inline ret_t                                                                                                   \
        apply(server::GameState &state,                                                                                \
              std::optional<std::unique_ptr<shared::ActionDecision>> action_decision = std::nullopt) const;            \
    };                                                                                                                 \
    inline BehaviourBase::ret_t name::apply(server::GameState &game_state,                                             \
                                            std::optional<std::unique_ptr<shared::ActionDecision>> action_decision)    \
            const

#define DEFINE_TEMPLATED_BEHAVIOUR(name, types, template_type, template_name)                                          \
    template <template_type template_name>                                                                             \
    class name : public BehaviourBase                                                                                  \
    {                                                                                                                  \
    public:                                                                                                            \
        inline ret_t                                                                                                   \
        apply(server::GameState &state,                                                                                \
              std::optional<std::unique_ptr<shared::ActionDecision>> action_decision = std::nullopt) const;            \
    };                                                                                                                 \
    template <template_type template_name>                                                                             \
    inline BehaviourBase::ret_t name<template_name>::apply(                                                            \
            server::GameState &game_state, std::optional<std::unique_ptr<shared::ActionDecision>> action_decision)     \
            const

// ================================
// HELPER MACROS
// ================================

// call this at the top of your behaviour to log the call.
#define LOG_CALL LOG(INFO) << "Applying " << CLASS_NAME

// call this if the linter is beeing a lil bitch.
#define SUPPRESS_UNUSED_VAR_WARNING(variable) (void)(variable)

#define ASSERT_DECISION                                                                                                \
    if ( action_decision == std::nullopt ) {                                                                           \
        LOG(ERROR) << "Expected a decision, but didnt receive one";                                                    \
        throw std::runtime_error("Expected a decision, but didnt receive one");                                        \
    }

#define ASSERT_NO_DECISION                                                                                             \
    if ( action_decision != std::nullopt ) {                                                                           \
        LOG(ERROR) << "Received a decision, but didnt excpect one";                                                    \
        throw std::runtime_error("Received a decision, but didnt excpect one");                                        \
    }

/**
 * @brief can be used like:
 * auto casted_decision = TRY_CAST_DECISION(expected_type);
 */
#define TRY_CAST_DECISION(type)                                                                                        \
    [](std::optional<std::unique_ptr<shared::ActionDecision>> &action_decision) -> type *                              \
    {                                                                                                                  \
        ASSERT_DECISION                                                                                                \
        auto *casted_decision = dynamic_cast<type *>(action_decision->get());                                          \
        if ( !casted_decision ) {                                                                                      \
            LOG(ERROR) << "Decision has wrong type! Expected: " << utils::demangle(typeid(type).name())                \
                       << ", but got: " << utils::demangle(typeid(*action_decision->get()).name());                    \
            throw std::runtime_error("Decision has wrong type");                                                       \
        }                                                                                                              \
        return casted_decision;                                                                                        \
    }(action_decision)


        // ================================
        // BEHAVIOUR IMPLEMENTATIONS
        // ================================
        DEFINE_TEMPLATED_BEHAVIOUR(GainCoins, BehaviourType::NON_INTERACTIVE, int, coins)
        {
            LOG_CALL;
            ASSERT_NO_DECISION;

            auto &affected_player = game_state.get_current_player();
            affected_player.incTreasure(coins);
            return std::nullopt;
        }

        DEFINE_TEMPLATED_BEHAVIOUR(GainBuys, BehaviourType::NON_INTERACTIVE, int, buys)
        {
            LOG_CALL;
            ASSERT_NO_DECISION;

            auto &affected_player = game_state.get_current_player();
            affected_player.incBuys(buys);
            return std::nullopt;
        }

        DEFINE_TEMPLATED_BEHAVIOUR(GainActions, BehaviourType::NON_INTERACTIVE, int, actions)
        {
            LOG_CALL;
            ASSERT_NO_DECISION;

            auto &affected_player = game_state.get_current_player();
            affected_player.incActions(actions);
            return std::nullopt;
        }

        DEFINE_TEMPLATED_BEHAVIOUR(GainPoints, BehaviourType::NON_INTERACTIVE, int, points)
        {
            LOG_CALL;
            ASSERT_NO_DECISION;

            auto &affected_player = game_state.get_current_player();
            affected_player.incPoints(points);
            return std::nullopt;
        }

        DEFINE_TEMPLATED_BEHAVIOUR(DrawCards, BehaviourType::NON_INTERACTIVE, int, n_cards)
        {
            LOG_CALL;
            ASSERT_NO_DECISION;

            auto &affected_player = game_state.get_current_player();
            affected_player.draw(n_cards);

            // assuming the state is updated automatically from the lobby/game_interface
            return std::nullopt;
        }

#define TODO_IMPLEMENT_ME                                                                                              \
    SUPPRESS_UNUSED_VAR_WARNING(game_state);                                                                           \
    SUPPRESS_UNUSED_VAR_WARNING(action_decision);                                                                      \
    LOG(ERROR) << "BEHAVIOUR " << CLASS_NAME << " IS NOT IMPLEMENTED YET";                                             \
    throw std::runtime_error("not implemented");                                                                       \
    return std::nullopt

        // ================================
        // PLACEHOLDER BEHAVIOUR
        // ================================
        DEFINE_BEHAVIOUR(NOT_IMPLEMENTED_YET, BehaviourType::NON_INTERACTIVE) { TODO_IMPLEMENT_ME; }

        // ================================
        // TODO: add behaviours
        // ================================

#undef DEFINE_BEHAVIOUR
#undef DEFINE_TEMPLATED_BEHAVIOUR
#undef TRY_CAST_DECISION
#undef LOG_CALL
#undef SUPPRESS_UNUSED_VAR_WARNING
#undef ASSERT_DECISION
#undef ASSERT_NO_DECISION
    } // namespace behaviour
} // namespace server
