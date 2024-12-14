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
        std::sort(this->hand_cards.begin(), this->hand_cards.end(),
                  [](const auto &id_a, const auto &id_b)
                  {
                      const auto type_a = shared::CardFactory::getType(id_a);
                      const auto type_b = shared::CardFactory::getType(id_b);

                      // custom order
                      auto getCustomOrder = [](shared::CardType type)
                      {
                          if ( (type & shared::CardType::ACTION) != 0 ) {
                              return 1;
                          } else if ( (type & shared::CardType::TREASURE) != 0 ) {
                              return 2;
                          } else if ( (type & shared::CardType::VICTORY) != 0 ) {
                              return 3;
                          } else {
                              return 4;
                          }
                      };

                      int order_a = getCustomOrder(type_a);
                      int order_b = getCustomOrder(type_b);

                      if ( order_a != order_b ) {
                          return order_a < order_b;
                      }

                      const auto cost_a = shared::CardFactory::getCost(id_a);
                      const auto cost_b = shared::CardFactory::getCost(id_b);

                      if ( cost_a != cost_b ) {
                          return cost_a < cost_b; // lowest cost first
                      }

                      // sort by name if same category and same cost
                      return id_a < id_b;
                  });

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
        if ( !staged_cards.empty() ) {
            LOG(ERROR) << "staged cards should be empty when getting deck";
            throw exception::OutOfPhase("You can not get your deck while you are playing a card!");
        }

        std::vector<shared::CardBase::id_t> deck;
        deck.reserve(draw_pile.size() + discard_pile.size() + hand_cards.size());
        deck.insert(deck.end(), draw_pile.begin(), draw_pile.end());
        deck.insert(deck.end(), discard_pile.begin(), discard_pile.end());
        deck.insert(deck.end(), hand_cards.begin(), hand_cards.end());

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
            // this should actually be unreachable because of the checks in GameState/GameInterface
            LOG(ERROR) << "staged cards should be empty when ending a turn";
            throw exception::OutOfPhase("You can not end your turn while you are playing a card!");
        }

        resetValues();

        move<shared::HAND, shared::DISCARD_PILE>();

        draw(5);
    }

    void Player::playAvailableTreasureCards()
    {
        auto cards_to_play = getType<shared::CardAccess::HAND>(shared::CardType::TREASURE);
        take<shared::HAND>(cards_to_play);
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
