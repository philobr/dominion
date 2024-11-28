#include <algorithm>
#include <random>
#include <ranges>

#include <server/game/server_player.h>
#include <shared/utils/assert.h>

namespace server
{

    bool Player::hasCardInHand(const shared::CardBase::id_t &card_id) const
    {
        return std::find(hand_cards.begin(), hand_cards.end(), card_id) != hand_cards.end();
    }

    bool Player::hasCardStaged(const shared::CardBase::id_t &card_id) const
    {
        return std::find(staged_cards.begin(), staged_cards.end(), card_id) != staged_cards.end();
    }

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
        resetValues();

        move<shared::HAND, shared::DISCARD_PILE>();
        move<shared::PLAYED_CARDS, shared::DISCARD_PILE>();

        draw(5);
    }

} // namespace server
