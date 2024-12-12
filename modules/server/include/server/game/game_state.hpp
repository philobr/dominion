#pragma once

#include "game_state.h"

template <enum shared::CardAccess FROM>
void server::GameState::tryPlay(const shared::PlayerBase::id_t &requestor_id, const shared::CardBase::id_t &card_id)
{
    if constexpr ( FROM != shared::CardAccess::HAND && FROM == shared::CardAccess::STAGED_CARDS ) {
        LOG(ERROR) << "Cards can only be played from " << toString(shared::CardAccess::HAND) << " or from "
                   << toString(shared::CardAccess::STAGED_CARDS) << ". " << toString(FROM) << " is not allowed!";
        static_assert(false); // this is on purpose, this way this fails to compile and the error can not go unnoticed
    }

    if ( requestor_id != getCurrentPlayerId() ) {
        LOG(WARN) << "Player " << requestor_id << " attempted to play a card out of turn.";
        throw exception::InvalidRequest("Not your turn.");
    }

    if constexpr ( FROM == shared::CardAccess::HAND ) {
        if ( phase != shared::GamePhase::ACTION_PHASE ) {
            LOG(WARN) << "Player " << requestor_id << " attempted to play card " << card_id << " during "
                      << gamePhaseToString(phase);
            throw exception::OutOfPhase("Cannot play cards while in " + gamePhaseToString(phase));
        }
    } else if constexpr ( FROM == shared::CardAccess::STAGED_CARDS ) {
        if ( phase != shared::GamePhase::PLAYING_ACTION_CARD ) {
            LOG(WARN) << "Player " << requestor_id << " attempted to play staged card " << card_id << " during "
                      << gamePhaseToString(phase);
            throw exception::OutOfPhase("Cannot play staged cards while in " + gamePhaseToString(phase));
        }
    }

    if constexpr ( FROM == shared::CardAccess::HAND ) {
        if ( getPlayer(requestor_id).getActions() == 0 ) {
            LOG(WARN) << "Player " << requestor_id << " attempted to play card " << card_id << " with no actions left.";
            throw exception::OutOfActions();
        }
    }

    if ( !getPlayer(requestor_id).hasCard<FROM>(card_id) ) {
        LOG(WARN) << "Player " << requestor_id << " attempted to play card " << card_id << " not in " << toString(FROM);
        throw exception::CardNotAvailable();
    }

    auto &player = getPlayer(requestor_id);
    player.move<FROM, shared::PLAYED_CARDS>(card_id);
    if constexpr ( FROM == shared::CardAccess::HAND ) {
        player.decActions();
    }

    board->addToPlayedCards(card_id);

    LOG(INFO) << "Player " << requestor_id << " successfully played card " << card_id << " from " << toString(FROM);
}

template <enum shared::CardAccess TO>
void server::GameState::tryGain(const shared::PlayerBase::id_t &requestor_id, const shared::CardBase::id_t &card_id)
{
    if constexpr ( TO != shared::HAND && TO != shared::DISCARD_PILE ) {
        LOG(ERROR) << "Cards can only be gained to " << toString(shared::HAND) << " or to "
                   << toString(shared::STAGED_CARDS) << ". " << toString(TO) << " is not allowed!";
        static_assert(false); // this is on purpose, this way this fails to compile and the error can not go
                              // unnoticed
    }

    if ( phase != shared::GamePhase::PLAYING_ACTION_CARD ) {
        LOG(WARN) << "Player " << requestor_id << " attempted to gain a card with ID: \'" << card_id
                  << "\' outside of the action card phase.";
        throw exception::OutOfPhase("Cannot gain a card while in " + gamePhaseToString(phase));
    }

    board->tryTake(card_id);
    auto &player = getPlayer(requestor_id);
    player.add<TO>(card_id);

    LOG(INFO) << "Player " << requestor_id << " successfully gained card " << card_id << " to " << toString(TO);
}