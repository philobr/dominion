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
            /**
             * @brief Used to store if a behaviour is finished.
             * Behaviours can return an empty OrderResponse, but still not be finished, hence this flag.
             */
            bool finished_behaviour;

        public:
            using ret_t = OrderResponse;
            using action_decision_t = std::optional<std::unique_ptr<shared::ActionDecision>>;

            Behaviour() : finished_behaviour(false) {}
            virtual ~Behaviour() = default;

            virtual ret_t apply(server::GameState &state, action_decision_t action_decision = std::nullopt) = 0;

            /**
             * @brief Can be called after a behaviour returns something from apply to check if its done or if there are
             * more steps.
             */
            bool isDone() const { return finished_behaviour; }
        };
    } // namespace base
} // namespace server

#include <server/game/behaviours_impl.hpp>