#pragma once

#include "game_state.h"

template <enum shared::CardAccess FROM>
inline void server::GameState::tryPlay(const shared::PlayerBase::id_t &requestor_id,
                                       const shared::CardBase::id_t &card_id)
{
    if constexpr ( FROM != shared::CardAccess::HAND && FROM != shared::CardAccess::STAGED_CARDS ) {
        LOG(ERROR) << "Cards can only be played from " << toString(shared::CardAccess::HAND) << " or from "
                   << toString(shared::CardAccess::STAGED_CARDS) << ". " << toString(FROM) << " is not allowed!";
        static_assert((FROM == shared::CardAccess::HAND || FROM == shared::CardAccess::STAGED_CARDS) &&
                      "provided CardAccess is not allowed!"); // this is on purpose, this way this fails to
                                                              // compile and the error can not go unnoticed
    }

    guaranteeIsCurrentPlayer(requestor_id, FUNC_NAME);

    if constexpr ( FROM == shared::CardAccess::HAND ) {
        guaranteePhase(requestor_id, card_id, shared::GamePhase::ACTION_PHASE, "You can not play a card", FUNC_NAME);

        if ( getPlayer(requestor_id).getActions() == 0 ) {
            LOG(WARN) << "Player \'" << requestor_id << "\' attempted to play card \'" << card_id
                      << "\' with no actions left.";
            throw exception::OutOfActions();
        }
    } else if constexpr ( FROM == shared::CardAccess::STAGED_CARDS ) {
        guaranteePhase(requestor_id, card_id, shared::GamePhase::PLAYING_ACTION_CARD, "You can not play a card",
                       FUNC_NAME);
    }

    if ( !getPlayer(requestor_id).hasCard<FROM>(card_id) ) {
        LOG(WARN) << "Player \'" << requestor_id << "\' attempted to play card \'" << card_id << "\' not in "
                  << toString(FROM);
        throw exception::CardNotAvailable();
    }

    auto &player = getPlayer(requestor_id);
    player.move<FROM, shared::PLAYED_CARDS>(card_id);
    if constexpr ( FROM == shared::CardAccess::HAND ) {
        player.decActions();
    }

    board->addToPlayedCards(card_id);

    printSuccess(requestor_id, FUNC_NAME);
}

template <enum shared::CardAccess TO>
inline void server::GameState::tryGain(const shared::PlayerBase::id_t &requestor_id,
                                       const shared::CardBase::id_t &card_id)
{
    if constexpr ( TO != shared::HAND && TO != shared::DISCARD_PILE ) {
        LOG(ERROR) << "Cards can only be gained to " << toString(shared::HAND) << " or to "
                   << toString(shared::STAGED_CARDS) << ". " << toString(TO) << " is not allowed!";
        static_assert((TO == shared::HAND || TO == shared::DISCARD_PILE) &&
                      "CardAccess not allowed!"); // this is on purpose, this way this fails to
                                                  // compile and the error can not go unnoticed
    }

    guaranteePhase(requestor_id, card_id, shared::GamePhase::PLAYING_ACTION_CARD, "You can not gain a card", FUNC_NAME);

    board->tryTake(card_id);
    auto &player = getPlayer(requestor_id);
    player.add<TO>(card_id);

    printSuccess(requestor_id, FUNC_NAME);
}