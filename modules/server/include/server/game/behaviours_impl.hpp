#include <server/game/behaviour_base.h>
#include <shared/utils/utils.h>

namespace server
{
    namespace behaviour
    {
        // TODO: define a stateful behaviour, ask @aaron if you need this

// False positive of clang-tidy
// NOLINTBEGIN(bugprone-macro-parentheses)
#define DEFINE_BEHAVIOUR(name)                                                                                         \
    class name : public server::base::Behaviour                                                                        \
    {                                                                                                                  \
    public:                                                                                                            \
        inline ret_t apply(server::GameState &state,                                                                   \
                           server::base::Behaviour::action_decision_t action_decision = std::nullopt);                 \
    };                                                                                                                 \
    inline server::base::Behaviour::ret_t name::apply(server::GameState &game_state,                                   \
                                                      server::base::Behaviour::action_decision_t action_decision)
// NOLINTEND(bugprone-macro-parentheses)

// False positive of clang-tidy
// NOLINTBEGIN(bugprone-macro-parentheses)
#define DEFINE_TEMPLATED_BEHAVIOUR(name, template_type, template_name)                                                 \
    template <template_type template_name>                                                                             \
    class name : public server::base::Behaviour                                                                        \
    {                                                                                                                  \
    public:                                                                                                            \
        inline ret_t apply(server::GameState &state,                                                                   \
                           server::base::Behaviour::action_decision_t action_decision = std::nullopt);                 \
    };                                                                                                                 \
    template <template_type template_name>                                                                             \
    inline server::base::Behaviour::ret_t name<template_name>::apply(                                                  \
            server::GameState &game_state, server::base::Behaviour::action_decision_t action_decision)
// NOLINTEND(bugprone-macro-parentheses)

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
#define TRY_CAST_DECISION(decision_type)                                                                               \
    [](server::base::Behaviour::action_decision_t &action_decision) -> decision_type *                                 \
    {                                                                                                                  \
        ASSERT_DECISION                                                                                                \
        auto *casted_decision = dynamic_cast<(decision_type) *>(action_decision->get());                               \
        if ( !casted_decision ) {                                                                                      \
            LOG(ERROR) << "Decision has wrong type! Expected: " << utils::demangle(typeid(decision_type).name())       \
                       << ", but got: " << utils::demangle(typeid(*action_decision->get()).name());                    \
            throw std::runtime_error("Decision has wrong type");                                                       \
        }                                                                                                              \
        return casted_decision;                                                                                        \
    }(action_decision)

#define BEHAVIOUR_DONE                                                                                                 \
    finished_behaviour = true;                                                                                         \
    return OrderResponse()


        // ================================
        // BEHAVIOUR IMPLEMENTATIONS
        // ================================
        DEFINE_TEMPLATED_BEHAVIOUR(GainCoins, int, coins)
        {
            LOG_CALL;
            ASSERT_NO_DECISION;

            auto &affected_player = game_state.getCurrentPlayer();
            affected_player.addTreasure(coins);

            BEHAVIOUR_DONE;
        }

        DEFINE_TEMPLATED_BEHAVIOUR(GainBuys, int, buys)
        {
            LOG_CALL;
            ASSERT_NO_DECISION;

            auto &affected_player = game_state.getCurrentPlayer();
            affected_player.addBuys(buys);

            BEHAVIOUR_DONE;
        }

        DEFINE_TEMPLATED_BEHAVIOUR(GainActions, int, actions)
        {
            LOG_CALL;
            ASSERT_NO_DECISION;

            auto &affected_player = game_state.getCurrentPlayer();
            affected_player.addActions(actions);

            BEHAVIOUR_DONE;
        }

        DEFINE_TEMPLATED_BEHAVIOUR(GainPoints, int, points)
        {
            LOG_CALL;
            ASSERT_NO_DECISION;

            auto &affected_player = game_state.getCurrentPlayer();
            affected_player.addPoints(points);

            BEHAVIOUR_DONE;
        }

        DEFINE_TEMPLATED_BEHAVIOUR(DrawCards, int, n_cards)
        {
            LOG_CALL;
            ASSERT_NO_DECISION;

            auto &affected_player = game_state.getCurrentPlayer();
            affected_player.draw(n_cards);

            BEHAVIOUR_DONE;
        }

#define TODO_IMPLEMENT_ME                                                                                              \
    SUPPRESS_UNUSED_VAR_WARNING(game_state);                                                                           \
    SUPPRESS_UNUSED_VAR_WARNING(action_decision);                                                                      \
    LOG(ERROR) << "BEHAVIOUR " << CLASS_NAME << " IS NOT IMPLEMENTED YET";                                             \
    throw std::runtime_error("not implemented");                                                                       \
    return OrderResponse()

        // ================================
        // PLACEHOLDER BEHAVIOUR
        // ================================
        DEFINE_BEHAVIOUR(NOT_IMPLEMENTED_YET) { TODO_IMPLEMENT_ME; }

        // ================================
        // TODO: add behaviours
        // ================================

#undef DEFINE_BEHAVIOUR
#undef BEHAVIOUR_DONE
#undef DEFINE_TEMPLATED_BEHAVIOUR
#undef TRY_CAST_DECISION
#undef LOG_CALL
#undef SUPPRESS_UNUSED_VAR_WARNING
#undef ASSERT_DECISION
#undef ASSERT_NO_DECISION
    } // namespace behaviour
} // namespace server
