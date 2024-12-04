#include <algorithm>
#include <random>
#include <ranges>

#include <server/game/server_player.h>
#include <shared/utils/assert.h>

namespace server
{
    reduced::Player::ptr_t Player::getReducedPlayer()
    {
        return reduced::Player::make(static_cast<shared::PlayerBase>(*this), hand_cards);
    }

    reduced::Enemy::ptr_t Player::getReducedEnemy()
    {
        return reduced::Enemy::make(static_cast<shared::PlayerBase>(*this), hand_cards.size());
    }

    void Player::resetValues()
    {
        actions = 1;
        buys = 1;
        treasure = 0;
        victory_points = 0;
    }

    void Player::endTurn()
    {
        if ( !staged_cards.empty() ) {
            LOG(ERROR) << "staged cards should be empty when ending a turn";
            throw std::runtime_error("tried to end a turn while staged cards were not empty");
        }

        resetValues();

        move<shared::HAND, shared::DISCARD_PILE>();
        move<shared::PLAYED_CARDS, shared::DISCARD_PILE>();

        draw(5);
    }

    void Player::playAvailableTreasureCards()
    {
        for ( const auto &card_id : getType<shared::CardAccess::HAND>(shared::CardType::TREASURE) ) {
            playCardFromHand(card_id);
        }
    }

} // namespace server
