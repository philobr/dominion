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
        LOG(ERROR) << "BehaviourList is already in use for card: \'" << card_id << "\'.Error in " << FUNC_NAME;
        throw exception::UnreachableCode();
    }

    LOG(DEBUG) << "Loading Behaviours for card \'" << card_id << "\'";
    behaviour_idx = 0;
    current_card = card_id;
    behaviour_list = behaviour_registry->getBehaviours(card_id);
}

void server::BehaviourChain::resetBehaviours()
{
    if ( hasNext() ) {
        LOG(ERROR) << "Tried to reset the BehaviourChain while it was still used!. Error in " << FUNC_NAME;
        throw exception::UnreachableCode();
    }

    LOG(DEBUG) << "Clearing behaviours for card \'" << current_card << "\'";

    behaviour_idx = 0;
    current_card.clear();
    behaviour_list.clear();
}

server::BehaviourChain::ret_t server::BehaviourChain::startChain(server::GameState &game_state)
{
    if ( empty() ) {
        LOG(ERROR) << "Tried to use an empty BehaviourChain, crashing now. Error in " << FUNC_NAME;
        throw exception::UnreachableCode();
    }

    return runBehaviourChain(game_state);
}

server::BehaviourChain::ret_t server::BehaviourChain::runBehaviourChain(server::GameState &game_state)
{
    LOG(INFO) << "Called " << FUNC_NAME << "for card \'" << current_card << "\'";
    while ( hasNext() ) {
        auto action_order = currentBehaviour().apply(game_state, game_state.getCurrentPlayerId(), std::nullopt);

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
server::BehaviourChain::continueChain(server::GameState &game_state, const shared::PlayerBase::id_t &player_id,
                                      std::unique_ptr<shared::ActionDecision> &action_decision)
{
    LOG(INFO) << "Called " << FUNC_NAME << "for card \'" << current_card << "\'";
    if ( empty() ) {
        LOG(ERROR) << "Tried to use an empty BehaviourChain. Client has a state mismatch.";
        throw exception::UnreachableCode();
    }

    auto action_order = currentBehaviour().apply(game_state, player_id, std::move(action_decision));

    if ( !currentBehaviour().isDone() ) {
        // can be an empty OrderResponse as well
        return action_order;
    }

    advance();
    return runBehaviourChain(game_state);
}