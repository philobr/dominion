#pragma once
#include <server/game/game_state/game_state.h>
#include <shared/message_types.h>
#include <shared/utils/logger.h>

enum BehaviourType
{
    NON_INTERACTIVE = 1,
    DEMANDS_ACTION = 2,
    EXPECTS_RESPONSE = 4,

    ORDER_ONLY = NON_INTERACTIVE | DEMANDS_ACTION,
    INTERACTIVE = DEMANDS_ACTION | EXPECTS_RESPONSE
};

/**
 * @brief A behaviour as described in the card refactor proposal.
 * Behaviours can still be created using templates (e.g. GainCoins<int coins=2>)
 */
class BehaviourBase
{
protected:
    const BehaviourType behaviour_type;

public:
    using ret_t = std::optional<std::unique_ptr<shared::ActionOrder>>;

    explicit BehaviourBase(BehaviourType type) : behaviour_type(type) {}
    virtual ~BehaviourBase() = default;

    virtual ret_t
    apply(server::GameState *state,
          std::optional<std::unique_ptr<shared::ActionDecision>> action_decision = std::nullopt) const = 0;

    bool expectsResponse() const { return (behaviour_type & BehaviourType::EXPECTS_RESPONSE) != 0; }

private:
#define CHECK_HAS_DECISION                                                                                             \
    if ( action_decision == std::nullopt ) {                                                                           \
        LOG(WARN) << "Expected a decision, but didnt receive one";                                                     \
        throw std::runtime_error("Expected a decision, but didnt receive one");                                        \
    }

#define CHECK_HAS_NO_DECISION                                                                                          \
    if ( action_decision != std::nullopt ) {                                                                           \
        LOG(WARN) << "Received a decision, but didnt excpect one";                                                     \
        throw std::runtime_error("Received a decision, but didnt excpect one");                                        \
    }

#define TRY_CAST_DECISION(type)                                                                                        \
    CHECK_HAS_DECISION                                                                                                 \
    if ( /* try to cast the decision */ ) {                                                                            \
        LOG(WARN) << "Decision has wrong type";                                                                        \
        throw std::runtime_error("Decision has wrong type");                                                           \
    } else {                                                                                                           \
        /* return the decision */                                                                                      \
    }
};

/**
 *
 *
 *
 * @brief the stuff down below is only experimental, i think this is probably a bad idea
 *
 *
 *
 */


// we can add a template <xy> here
#define DEFINE_BEHAVIOUR(name, types)                                                                                  \
    class name : public BehaviourBase                                                                                  \
    {                                                                                                                  \
        const BehaviourType behaviour_type = (types);                                                                  \
                                                                                                                       \
    public:                                                                                                            \
        ret_t apply(server::GameState *state,                                                                          \
                    std::optional<std::unique_ptr<shared::ActionDecision>> action_decision = std::nullopt) const;      \
                                                                                                                       \
        bool expectsResponse() const { return (behaviour_type & BehaviourType::EXPECTS_RESPONSE) != 0; }               \
    }

template <int coins>
DEFINE_BEHAVIOUR(GainCoins, BehaviourType::NON_INTERACTIVE);


#define IMPLEMENT_BEHAVIOUR                                                                                            \
    apply(server::GameState *state,                                                                                    \
          std::optional<std::unique_ptr<shared::ActionDecision>> action_decision = std::nullopt) const

template <int coins>
GainCoins<coins>::ret_t GainCoins<coins>::IMPLEMENT_BEHAVIOUR
{
    CHECK_HAS_NO_DECISION;
}
