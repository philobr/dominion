#include <server/game/behaviour_chain.h>

void server::BehaviourChain::loadBehaviours(const std::string &card_id)
{
    if ( !empty() ) {
        LOG(WARN) << "BehaviourList is already in use!";
        throw std::runtime_error("BehaviourList is already in use!");
    }

    LOG(DEBUG) << "Loading Behaviours for card_id(" << card_id << ")";

    behaviour_idx = 0;
    current_card = card_id;
    behaviour_list = behaviour_registry->getBehaviours(card_id);
}

void server::BehaviourChain::resetBehaviours()
{
    if ( hasNext() ) {
        LOG(WARN) << "Tried to reset the BehaviourChain while it was still used!";
        return;
    }

    behaviour_idx = INVALID_IDX;
    current_card = INVALID_CARD;
    behaviour_list.clear();
}

server::BehaviourChain::ret_t server::BehaviourChain::start(server::GameState &game_state)
{
    if ( empty() ) {
        LOG(ERROR) << "Tried to use an empty BehaviourChain, crashing now";
        throw std::runtime_error("Unreachable Code");
    }

    while ( hasNext() ) {
        auto action_order = currentBehaviour().apply(game_state, std::nullopt);

        if ( currentBehaviour().isDone() ) {
            advance();
        } else {
            return action_order;
        }
    }

    // behaviour chain is done
    resetBehaviours();
    return std::nullopt;
}

server::BehaviourChain::ret_t
server::BehaviourChain::receiveAction(server::GameState &game_state,
                                      std::unique_ptr<shared::ActionDecision> &action_decision)
{
    if ( empty() ) {
        LOG(ERROR) << "Tried to use an empty BehaviourChain, crashing now";
        throw std::runtime_error("Unreachable Code");
    }

    auto action_order = currentBehaviour().apply(game_state, std::move(action_decision));

    if ( !currentBehaviour().isDone() ) {
        return action_order;
    }

    advance();
    return start(game_state);
}