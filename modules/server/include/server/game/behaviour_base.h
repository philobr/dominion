#pragma once
#include <server/game/game_state.h>
#include <shared/message_types.h>
#include <shared/utils/logger.h>

namespace server
{
    enum BehaviourType
    {
        NONE,
        NON_INTERACTIVE = 1,
        DEMANDS_ACTION = 2,
        EXPECTS_RESPONSE = 4,

        ORDER_ONLY = NON_INTERACTIVE | DEMANDS_ACTION,
        INTERACTIVE = DEMANDS_ACTION | EXPECTS_RESPONSE
    };

    class BehaviourBase
    {
    protected:
        const BehaviourType behaviour_type;

    public:
        using ret_t = std::optional<std::unique_ptr<shared::ActionOrder>>;

        BehaviourBase() : behaviour_type(BehaviourType::NONE) {}
        BehaviourBase(BehaviourType type) : behaviour_type(type) {}
        virtual ~BehaviourBase() = default;

        virtual ret_t
        apply(server::GameState &state,
              std::optional<std::unique_ptr<shared::ActionDecision>> action_decision = std::nullopt) const = 0;

        // i think we dont even need this
        bool expectsResponse() const { return (behaviour_type & BehaviourType::EXPECTS_RESPONSE) != 0; }
    };

} // namespace server

#include <server/game/behaviours_impl.hpp>