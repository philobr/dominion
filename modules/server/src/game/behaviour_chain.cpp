#include <server/game/behaviour_chain.h>
#include <shared/utils/logger.h>

server::BehaviourChain::BehaviourChain() :
    current_card(""), behaviour_idx(0), behaviour_registry(std::make_unique<BehaviourRegistry>())
{
    LOG(DEBUG) << "Created a new BehaviourChain";
}

void server::BehaviourChain::loadBehaviours(const std::string &card_id)
{
    if ( !empty() ) {
        LOG(WARN) << "BehaviourList is already in use for card: " << card_id;
        throw std::runtime_error("BehaviourList is already in use!");
    }

    LOG(DEBUG) << "Loading Behaviours for card:" << card_id;
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

    behaviour_idx = 0;
    current_card.clear();
    behaviour_list.clear();
}

server::BehaviourChain::ret_t server::BehaviourChain::startChain(server::GameState &game_state)
{
    if ( empty() ) {
        LOG(ERROR) << "Tried to use an empty BehaviourChain, crashing now";
        throw std::runtime_error("Unreachable Code");
    }

    return runBehaviourChain(game_state);
}

server::BehaviourChain::ret_t server::BehaviourChain::runBehaviourChain(server::GameState &game_state)
{
    while ( hasNext() ) {
        auto action_order = currentBehaviour().apply(game_state, std::nullopt);

        if ( currentBehaviour().isDone() ) {
            advance();
        } else {
            // can be an empty OrderResponse as well
            return action_order;
        }
    }

    // we applied all behaviours
    resetBehaviours();
    return OrderResponse();
}

server::BehaviourChain::ret_t
server::BehaviourChain::continueChain(server::GameState &game_state,
                                      std::unique_ptr<shared::ActionDecision> &action_decision)
{
    if ( empty() ) {
        LOG(ERROR) << "Tried to use an empty BehaviourChain, crashing now";
        throw std::runtime_error("Unreachable Code");
    }

    auto action_order = currentBehaviour().apply(game_state, std::move(action_decision));

    if ( !currentBehaviour().isDone() ) {
        // can be an empty OrderResponse as well
        return action_order;
    }

    advance();
    return runBehaviourChain(game_state);
}