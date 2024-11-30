#pragma once

#include <server/game/behaviour_registry.h>
#include <shared/utils/logger.h>
#include <vector>

namespace server
{
    /**
     * @brief This class stores the state of the card that is currently being played. it will be accessed from the game
     * interface and handle incoming/outcoming messages. it will also apply the behaviours to the player/gamestate.
     */
    class BehaviourChain
    {
        static const size_t INVALID_IDX;
        static const std::string INVALID_CARD;

        std::string current_card;
        size_t behaviour_idx;
        std::unique_ptr<BehaviourRegistry> behaviour_registry;

        std::vector<std::unique_ptr<base::Behaviour>> behaviour_list;

    public:
        using ret_t = server::base::Behaviour::ret_t;

        BehaviourChain() :
            current_card(INVALID_CARD), behaviour_idx(INVALID_IDX),
            behaviour_registry(std::make_unique<BehaviourRegistry>())
        {
            LOG(DEBUG) << "Created a new BehaviourChain";
        }

        void loadBehaviours(const std::string &card_id);

        ret_t start(server::GameState &game_state);
        ret_t receiveAction(server::GameState &game_state, std::unique_ptr<shared::ActionDecision> &action_decision);

        bool empty() const { return behaviour_idx == INVALID_IDX && current_card == INVALID_CARD; }

    private:
        void resetBehaviours();

        inline void advance() { ++behaviour_idx; }
        inline void advanceIfDone()
        {
            if ( currentBehaviour().isDone() ) {
                advance();
            }
        }

        inline bool hasNext() const { return behaviour_idx < behaviour_list.size(); }

        base::Behaviour &currentBehaviour() { return *behaviour_list[behaviour_idx]; }

        ret_t applyBehaviour(server::GameState &game_state,
                             std::optional<std::unique_ptr<shared::ActionDecision>> action_decision = std::nullopt);
    };

    inline constexpr size_t BehaviourChain::INVALID_IDX = static_cast<size_t>(-1);
    inline const std::string BehaviourChain::INVALID_CARD = "INVALID_CARD";
} // namespace server
