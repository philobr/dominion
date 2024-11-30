#pragma once
#include <server/game/game_state.h>
#include <server/message/order_response.h>
#include <shared/message_types.h>
#include <shared/utils/logger.h>

namespace server
{
    namespace base
    {
        class Behaviour
        {
        protected:
            bool finished_behaviour = false;

        public:
            using ret_t = OrderResponse;
            using action_decision_t = std::optional<std::unique_ptr<shared::ActionDecision>>;

            Behaviour() = default;
            virtual ~Behaviour() = default;

            virtual ret_t apply(server::GameState &state, action_decision_t action_decision = std::nullopt) = 0;

            bool isDone() const { return finished_behaviour; }
        };
    } // namespace base
} // namespace server

#include <server/game/behaviours_impl.hpp>