#include <algorithm>
#include <random>
#include <ranges>

#include <server/game/behaviour_registry.h>
#include <server/game/server_player.h>
#include <shared/utils/assert.h>

namespace server
{
    reduced::Player::ptr_t Player::getReducedPlayer()
    {
        this->draw_pile_size = draw_pile.size();
        return reduced::Player::make(static_cast<shared::PlayerBase>(*this), hand_cards);
    }

    reduced::Enemy::ptr_t Player::getReducedEnemy()
    {
        this->draw_pile_size = draw_pile.size();
        return reduced::Enemy::make(static_cast<shared::PlayerBase>(*this), hand_cards.size());
    }

    std::vector<shared::CardBase::id_t> Player::getDeck() const
    {
        std::vector<shared::CardBase::id_t> deck;
        deck.reserve(draw_pile.size() + discard_pile.size() + hand_cards.size());
        deck.insert(deck.end(), draw_pile.begin(), draw_pile.end());
        deck.insert(deck.end(), discard_pile.begin(), discard_pile.end());
        deck.insert(deck.end(), hand_cards.begin(), hand_cards.end());
        if ( !staged_cards.empty() ) {
            LOG(ERROR) << "staged cards should be empty when getting deck";
            throw std::runtime_error("tried to get deck while staged cards were not empty");
        }
        if ( !played_cards.empty() ) {
            LOG(ERROR) << "played cards should be empty when getting deck";
            throw std::runtime_error("tried to get deck while played cards were not empty");
        }
        return deck;
    }

    void Player::resetValues()
    {
        actions = 1;
        buys = 1;
        treasure = 0;
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

    int Player::getVictoryPoints() const
    {
        std::vector<shared::CardBase::id_t> deck = getDeck();
        int victory_points = 0;
        for ( const auto &card_id : deck ) {
            VictoryCardBehaviour &behaviour = BehaviourRegistry().getVictoryBehaviour(card_id);
            victory_points += behaviour.getVictoryPoints(deck);
        }
        return victory_points;
    }

} // namespace server
