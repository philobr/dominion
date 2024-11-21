#pragma once

#include <server/game/cards/cards_new/behaviour.h>
#include <shared/utils/logger.h>
#include <vector>

/**
 * @brief BehaviourChain as discussed in MergeRequest Proposal/cards_redesign.
 * We could add:
 *
 * - A shared card_map where all cards are included with their base stats (id, type, cost)
 * - A server behaviour_map where card names are linked with
 *      - list of behaviour pointers (probably best)
 *      - either behaviour chains (cumbersome)
 *
 */
class BehaviourChain
{
    // maybe even raw pointers or const ref? we dont want ownership of behaviours
    std::vector<std::unique_ptr<BehaviourBase>> behaviour_list;
    size_t behaviour_idx;

public:
    BehaviourChain() : behaviour_idx(-1) { LOG(DEBUG) << "Created a new BehaviourChain"; }

    void innit(std::vector<std::unique_ptr<BehaviourBase>> new_behaviour_list);
    void load_behaviours(const std::string &card_name); // i prefer this to the one above, we directly access the
                                                        // behaviour_map through this function

    void reset();

    /**
     * @brief Using a raw pointer here as ownership transfer would be annoying.
     *
     * @param game_state
     * @param action_decision
     * @return std::optional<std::unique_ptr<shared::ActionOrder>>
     */
    std::optional<std::unique_ptr<shared::ActionOrder>>
    receiveAction(server::GameState *game_state,
                  std::optional<std::unique_ptr<shared::ActionDecision>> action_decision);

private:
    const BehaviourBase &getBehaviour() const { return *behaviour_list[behaviour_idx].get(); }

    void advance() { ++behaviour_idx; }
    bool hasNext() const { return behaviour_idx != behaviour_list.size(); }
    void reset() { behaviour_idx = 0; }

    std::optional<std::unique_ptr<shared::ActionOrder>>
    handleDecision(server::GameState *state, std::optional<std::unique_ptr<shared::ActionDecision>> action_decision);
};

void BehaviourChain::innit(std::vector<std::unique_ptr<BehaviourBase>> new_behaviour_list)
{
    if ( !behaviour_list.empty() || behaviour_idx != -1 ) {
        LOG(WARN) << "BehaviourList is already initialised!";
        throw std::runtime_error("BehaviourList is already initialised !");
    }

    behaviour_list = new_behaviour_list;
    behaviour_idx = 0;
}

void BehaviourChain::reset()
{
    if ( hasNext() ) {
        LOG(WARN) << "Tried to reset the BehaviourChain before it was finished!";
        return;
    }

    behaviour_idx = -1; // this way we throw if we try to use an uninitialised chain
    behaviour_list.clear();
}

std::optional<std::unique_ptr<shared::ActionOrder>>
BehaviourChain::receiveAction(server::GameState *game_state,
                              std::optional<std::unique_ptr<shared::ActionDecision>> action_decision)
{
    while ( hasNext() ) {
        auto action_order = action_decision.has_value() ? handleDecision(game_state, std::move(action_decision))
                                                        : getBehaviour().apply(game_state);

        if ( action_order != std::nullopt ) {
            return action_order;
        }

        advance();
    }

    return std::nullopt;
}

std::optional<std::unique_ptr<shared::ActionOrder>>
BehaviourChain::handleDecision(server::GameState *state,
                               std::optional<std::unique_ptr<shared::ActionDecision>> action_decision)
{
    // unneccessary, but better safe than sorry
    if ( !action_decision.has_value() ) {
        LOG(WARN) << "Expected an action_decision, but didnt get one!";
        throw std::runtime_error("Didnt receive a decision");
    }

    // we should not receive an action decision if the behaviour has this type
    if ( !getBehaviour().expectsResponse() ) {
        LOG(WARN) << "Behaviour type should not receive a response";
        throw std::runtime_error("Received a decision in the wrong state");
    }

    return getBehaviour().apply(state, std::move(action_decision));
}