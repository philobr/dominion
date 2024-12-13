#include <server/game/behaviour_base.h>

namespace server
{
    namespace behaviour
    {

        namespace helper
        {

            template <typename OrderGenerator>
            server::base::Behaviour::ret_t sendAttackToEnemies(GameState &game_state, OrderGenerator gen)
            {
                server::base::Behaviour::ret_t orders;
                const auto player_id = game_state.getCurrentPlayerId();
                const auto player_ids = game_state.getAllPlayerIDs();

                std::for_each(player_ids.begin(), player_ids.end(),
                              [&](const auto enemy_id)
                              {
                                  if ( enemy_id == player_id ) {
                                      return;
                                  }

                                  if ( game_state.getPlayer(enemy_id).canBlock() ) {
                                      return;
                                  }

                                  auto order = gen(game_state, enemy_id);

                                  if ( order == nullptr ) {
                                      return;
                                  }

                                  orders.addOrder(enemy_id, std::move(order));
                              });

                return orders;
            }

            template <typename AttackFunction>
            void applyAttackToEnemies(GameState &game_state, AttackFunction attack_func)
            {
                auto attacker_id = game_state.getCurrentPlayerId();
                const size_t player_count = game_state.getAllPlayerIDs().size();
                const auto all_player_ids = game_state.getAllPlayerIDs();
                // find the cur_player idx
                auto attacker_it = std::find(all_player_ids.begin(), all_player_ids.end(), attacker_id);
                size_t attacker_index = std::distance(all_player_ids.begin(), attacker_it);

                // starting at 1 to skip current player_id
                for ( size_t i = 1; i < player_count; ++i ) {
                    const size_t enemy_index = (i + attacker_index) % player_count;
                    const auto enemy_id = game_state.getAllPlayerIDs().at(enemy_index);

                    if ( game_state.getPlayer(enemy_id).canBlock() ) {
                        return;
                    }

                    try {
                        attack_func(game_state, enemy_id);
                    } catch ( const std::exception &e ) {
                        LOG(DEBUG) << e.what();
                        return;
                    }
                }
            }


            static inline auto validateResponse(GameState &game_state, const shared::PlayerBase::id_t &requestor_id,
                                                std::unique_ptr<shared::ActionDecision> &action_decision,
                                                unsigned int min_cards, unsigned int max_cards,
                                                shared::CardType expected_type = static_cast<shared::CardType>(
                                                        shared::CardType::ACTION | shared::CardType::ATTACK |
                                                        shared::CardType::CURSE | shared::CardType::KINGDOM |
                                                        shared::CardType::REACTION | shared::CardType::TREASURE |
                                                        shared::CardType::VICTORY))
            {
                const auto player_id = requestor_id;
                auto &player = game_state.getPlayer(player_id);
                const auto *deck_choice = dynamic_cast<shared::DeckChoiceDecision *>(action_decision.get());
                const auto choice_size = deck_choice->cards.size();

                // validate the decision type
                if ( deck_choice == nullptr ) {
                    LOG(ERROR) << FUNC_NAME << " got a wrong decision type! Expected: shared::DeckChoiceDecision";
                    throw std::runtime_error("Decision type is not allowed!");

                    const auto *decision_ptr = action_decision.get();
                    if ( decision_ptr != nullptr ) {
                        LOG(ERROR) << FUNC_NAME << " got a wrong decision type from player " << player_id
                                   << "! expected: shared::GainFromBoardDecision, got: "
                                   << utils::demangle(typeid(*decision_ptr).name());
                    } else {
                        LOG(ERROR) << FUNC_NAME << " got a null pointer for action decision!";
                    }

                    throw std::runtime_error("Decision type is not allowed!");
                }

                // validate number of cards
                if ( min_cards == max_cards ) {
                    // choose exactly
                    if ( (choice_size != min_cards) ) {
                        LOG(ERROR) << FUNC_NAME << "Expects exactly " << min_cards << ", but player " << player_id
                                   << " chose " << choice_size << " cards!";
                        throw std::runtime_error("You have to choose exactly " + std::to_string(min_cards) + "!");
                    }
                } else {
                    // choose in range
                    if ( choice_size < min_cards || choice_size > max_cards ) {
                        LOG(ERROR) << FUNC_NAME << "Expected between " << min_cards << " and " << max_cards
                                   << " cards, but player " << player_id << " chose " << choice_size << " cards!";
                        throw std::runtime_error("You have to choose between " + std::to_string(min_cards) + " and " +
                                                 std::to_string(max_cards) + " cards!");
                    }
                }

                // validate existence and type of the card
                for ( const auto &card_id : deck_choice->cards ) {
                    const auto card_type = shared::CardFactory::getType(card_id);

                    if ( (card_type & expected_type) != card_type ) {
                        LOG(ERROR) << FUNC_NAME << "Player: " << player_id << " chose card: " << card_id
                                   << ", which has the wrong type!";
                        throw std::runtime_error("Card type not allowed!");
                    }

                    if ( !player.hasCard<shared::CardAccess::HAND>(card_id) ) {
                        LOG(ERROR) << FUNC_NAME << "Player: " << player.getId() << " does not have card: " << card_id
                                   << " in hand!";
                        throw std::runtime_error("Card not in hand!");
                    }
                }

                return *deck_choice;
            }


        } // namespace helper
    } // namespace behaviour
} // namespace server
