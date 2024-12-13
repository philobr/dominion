#pragma once

#include <server/game/behaviour_registry.h>
#include <vector>

namespace server
{
    /**
     * @brief This class is used in the GameInterface to be store and run behaviours. We can load behaviours into this
     * class, which are then stored. Should a card contain multistep behaviours (cards that require interaction from one
     * or more users to continue) we can easily stop and continue where we left off. The class only does basic
     * error-handling, the more detailed handling will be done by the behaviours themselves.
     */
    class BehaviourChain
    {
        std::string current_card;
        size_t behaviour_idx;
        std::unique_ptr<BehaviourRegistry> behaviour_registry;

        std::vector<std::unique_ptr<base::Behaviour>> behaviour_list;

    public:
        using ret_t = server::base::Behaviour::ret_t;

        BehaviourChain();
        ~BehaviourChain() = default;

        void loadBehaviours(const std::string &card_id);

        /**
         * @brief This is called the first time we execute a behaviour.
         * First calls never include an action order.
         */
        ret_t startChain(server::GameState &game_state);

        /**
         * @brief If a card has multi-step behaviours we call this function to pass in the action_decision.
         */
        ret_t continueChain(server::GameState &game_state, std::unique_ptr<shared::ActionDecision> &action_decision);

        inline bool empty() const { return (behaviour_idx == 0) && current_card.empty() && behaviour_list.empty(); }

    private:
        inline void resetBehaviours();

        inline void advance() { ++behaviour_idx; }
        inline bool hasNext() const { return behaviour_idx < behaviour_list.size(); }

        base::Behaviour &currentBehaviour() { return *behaviour_list[behaviour_idx]; }

        /**
         * @warning Does not check (explicitly) if a behaviour is loaded, this happens in startChain and continueChain
         */
        inline ret_t runBehaviourChain(server::GameState &game_state);
    };
} // namespace server
