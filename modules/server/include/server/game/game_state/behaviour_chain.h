#pragma once

#include <server/game/game_state/cards_new/behaviour_registry.h>
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
 *
 * THIS CLASS WILL BE EXPOSED TO THE GAME_INTERFACE
 *
 */
class BehaviourChain
{
    std::string current_card;
    size_t behaviour_idx;
    std::unique_ptr<BehaviourRegistry> behaviour_registry;

public:
    BehaviourChain() : current_card(""), behaviour_idx(-1), behaviour_registry(std::make_unique<BehaviourRegistry>())
    {
        LOG(DEBUG) << "Created a new BehaviourChain";
    }

    void load_card(const std::string &card_id);
    void reset_card();

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
    const BehaviourBase &getBehaviour() const
    {
        return *behaviour_registry->getBehaviours(current_card)[behaviour_idx].get();
    }

    void advance() { ++behaviour_idx; }
    bool hasNext() const { return behaviour_idx != behaviour_registry->getBehaviours(current_card).size(); }
    void reset() { behaviour_idx = 0; }

    std::optional<std::unique_ptr<shared::ActionOrder>>
    handleDecision(server::GameState *state, std::optional<std::unique_ptr<shared::ActionDecision>> action_decision);
};

void BehaviourChain::load_card(const std::string &card_id)
{
    if ( !current_card.empty() || behaviour_idx != -1 ) {
        LOG(WARN) << "BehaviourList is already initialised!";
        throw std::runtime_error("BehaviourList is already initialised !");
    }

    behaviour_idx = 0;
    current_card = card_id;
}

void BehaviourChain::reset_card()
{
    if ( hasNext() ) {
        LOG(WARN) << "Tried to reset the BehaviourChain before it was finished!";
        return;
    }

    behaviour_idx = -1; // this way we throw if we try to use an uninitialised chain
    current_card.clear();
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