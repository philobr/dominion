#pragma once
#include <server/game/game_state.h>
#include <shared/message_types.h>
#include <shared/utils/logger.h>

namespace server
{
    namespace base
    {
        class Behaviour
        {
            bool finished_behaviour = false;

        public:
            using ret_t = std::optional<std::vector<std::unique_ptr<shared::ActionOrder>>>;
            using action_decision_t = std::optional<std::unique_ptr<shared::ActionDecision>>;

            Behaviour() = default;
            virtual ~Behaviour() = default;

            virtual ret_t apply(server::GameState &state, action_decision_t action_decision = std::nullopt) const = 0;

            bool isDone() const { return finished_behaviour; }
        };
    } // namespace base
} // namespace server

#include <server/game/behaviours_impl.hpp>