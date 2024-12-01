#pragma once

#include <server/game/behaviour_registry.h>
#include <vector>

namespace server
{
    /**
     * @brief This class stores the state of the card that is currently being played. it will be accessed from the game
     * interface and handle incoming/outcoming messages. it will also apply the behaviours to the player/gamestate.
     */
    class BehaviourChain
    {
        std::string current_card;
        size_t behaviour_idx;
        std::unique_ptr<BehaviourRegistry> behaviour_registry;

        std::vector<std::unique_ptr<base::Behaviour>> behaviour_list;

    public:
        using ret_t = server::base::Behaviour::ret_t;

        BehaviourChain() : current_card(""), behaviour_idx(0), behaviour_registry(std::make_unique<BehaviourRegistry>())
        {
            LOG(DEBUG) << "Created a new BehaviourChain";
        }

        void loadBehaviours(const std::string &card_id);

        /**
         * @brief This is called the first time we execute a behaviour.
         * First calls never include an action order.
         */
        ret_t start(server::GameState &game_state);

        /**
         * @brief If a card has multi-step behaviours we call this function to pass in the action_decision.
         */
        ret_t receiveAction(server::GameState &game_state, std::unique_ptr<shared::ActionDecision> &action_decision);

        inline bool empty() const { return (behaviour_idx == 0) && current_card.empty() && behaviour_list.empty(); }

    private:
        void resetBehaviours();

        inline void advance() { ++behaviour_idx; }
        inline bool hasNext() const { return behaviour_idx < behaviour_list.size(); }

        base::Behaviour &currentBehaviour() { return *behaviour_list[behaviour_idx]; }
    };
} // namespace server
