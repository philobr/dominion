#include <algorithm>
#include <random>

#include <server/game/game_state/game_state.h>

#include <shared/game/cards/card_factory.h>
#include <shared/utils/assert.h>
#include <shared/utils/exception.h>
#include <shared/utils/logger.h>

namespace server
{
    GameState::GameState(const std::vector<shared::CardBase::id_t> &play_cards,
                         const std::vector<Player::id_t> &player_ids) :
        current_player_idx(0),
        phase(GamePhase::ACTION_PHASE)
    {
        if ( player_ids.size() < 2 && player_ids.size() > 4 ) {
            LOG(ERROR) << "Should have 2-4 players, but got: " << player_ids.size();
            throw exception::PlayerCountMismatch("wrong player count!");
        }

        initialise_players(player_ids);
        initialise_board(play_cards);
    }

    GameState::GameState() = default;
    GameState::~GameState() = default;

    GameState::GameState(GameState &&other) :
        player_order(other.player_order), current_player_idx(other.current_player_idx)
    {
        for ( const auto &pair : other.player_map ) {
            player_map[pair.first] = std::make_unique<Player>(*pair.second);
        }

        if ( other.board ) {
            board = other.board;
        }
    }

    void GameState::initialise_players(const std::vector<Player::id_t> &player_ids)
    {
        player_order = player_ids; // for now the player order will be the same as the list of player ids
        for ( const auto &id : player_ids ) {
            if ( player_map.count(id) != 0u ) {
                LOG(ERROR) << "tried to add player(" << id << ") twice";
                throw exception::DuplicatePlayer("cant add the same player twice!");
            }

            player_map[id] = std::make_unique<Player>(id);

            for ( unsigned i = 0; i < 7; i++ ) {
                if ( i < 3 ) {
                    player_map[id]->gain("Estate");
                }
                player_map[id]->gain("Copper");
            }

            player_map[id]->draw(5);
        }
    }

    void GameState::initialise_board(const std::vector<shared::CardBase::id_t> &selected_cards)
    {
        if ( selected_cards.size() != size_t(10) ) {
            LOG(ERROR) << "Excepcted 10 cards but got " << selected_cards.size();
            throw exception::WrongCardCount("");
        }
        board = server::ServerBoard::make(selected_cards, player_map.size());
    }

    std::unique_ptr<shared::ReducedGameState> GameState::get_reduced_state(const Player::id_t &target_player)
    {
        std::vector<shared::ReducedEnemy::ptr_t> reduced_enemies;
        std::for_each(player_map.begin(), player_map.end(),
                      [&](auto &entry)
                      {
                          if ( auto &[player_id, player_ptr] = entry; player_id != target_player ) {
                              reduced_enemies.emplace_back(player_ptr->get_reduced_enemy());
                          }
                      });

        auto reduced_player = get_player(target_player).get_reduced_player();
        Player::id_t active_player_id = getCurrentPlayerId();
        shared::Board::ptr_t reduced_board = board->getReduced();

        return std::make_unique<shared::ReducedGameState>(reduced_board, std::move(reduced_player),
                                                          std::move(reduced_enemies), active_player_id);
    }

    bool GameState::try_buy(const Player::id_t &player_id, const shared::CardBase::id_t &card_id)
    {
        auto &player = get_player(player_id);
        const auto card_cost = shared::CardFactory::getCard(card_id).getCost();

        if ( player.getTreasure() < card_cost ) {
            LOG(ERROR) << player_id << " has " << player.getTreasure() << " coins but needs " << card_cost;
            throw exception::InsufficientFunds("");
        }

        if ( !board->buy(card_id) ) {
            LOG(ERROR) << card_id << " is not available";
            throw exception::CardNotAvailable("");
        }

        player.gain(card_id);
        player.decTreasure(card_cost);

        return true;
    }

    bool GameState::try_play(const Player::id_t &affected_player, size_t hand_index, shared::CardAccess from)
    {
        auto &player = get_player(affected_player);
        const auto &card_id = player.get<shared::CardAccess::HAND>()[hand_index];
        const auto &card = shared::CardFactory::getCard(card_id);

        if ( card.isAction() ) {
            if ( getPhase() != GamePhase::ACTION_PHASE ) {
                LOG(WARN) << "player(" << affected_player << ") is currently not in the action phase, retrying";
                return false;
            }
            if ( player.is_currently_playing_card() ) {
                LOG(WARN) << "player(" << affected_player << ") is already playing a different card, retrying";
                return false;
            }
            if ( player.getActions() == 0 ) {
                force_switch_phase();
                LOG(WARN) << "tried to play an action card, but has no actions left";
                return false;
            }
            if ( !player.has_card_playable(card_id) ) {
                LOG(ERROR) << "tried to play a card that is not in the hand or staged cards";
                throw exception::InvalidCardAccess("");
            }

            player.set_currently_playing_card(card_id);
            player.decActions();
            phase = GamePhase::PLAYING_ACTION_CARD;

            if ( from == shared::CardAccess::HAND ) {
                player.play_card_from_hand(hand_index);
            } else if ( from == shared::CardAccess::STAGED_CARDS ) {
                player.play_card_from_staged(hand_index);
            } else {
                LOG(ERROR) << "tried to play a card from an invalid pile";
                throw exception::InvalidCardAccess("");
            }
            return true;
        }

        LOG(WARN) << "tried to play a card that isn't an action card";

        return false;
    }

    void GameState::end_turn()
    {
        get_current_player().end_turn();
        switch_player();
        reset_phase();

        if ( is_game_over() ) {
            end_game();
        }
    }

    bool GameState::is_game_over() const { return board->isGameOver(); }

    bool GameState::validateKingdomCardTypes(const std::vector<shared::CardBase::id_t> &kingdom_cards)
    {
        return std::all_of(kingdom_cards.begin(), kingdom_cards.end(),
                           [](const auto &card_id)
                           {
                               const auto &card = shared::CardFactory::getCard(card_id);
                               return card.isAction() || card.isAttack() || card.isReaction();
                           });
    }

    void GameState::force_switch_phase()
    {
        switch ( phase ) {
            case GamePhase::ACTION_PHASE:
                {
                    phase = GamePhase::BUY_PHASE;
                }
                break;
            case GamePhase::BUY_PHASE:
                {
                    phase = GamePhase::ACTION_PHASE;
                }
                break;
            case GamePhase::PLAYING_ACTION_CARD:
                {
                    LOG(WARN) << "tried to switch from GamePhase::PLAYING_ACTION_CARD, which is not possible, you "
                                 "have to finish playing your card first";
                    throw exception::OutOfPhase("");
                }
            default:
                {
                    LOG(ERROR) << "tried to switch from GamePhase::" << static_cast<int>(phase)
                               << ", which does not exist";
                    throw std::runtime_error("unreachable code");
                }
        }
    }

    void GameState::maybe_switch_phase()
    {
        auto &player = get_current_player();
        switch ( phase ) {
            case GamePhase::ACTION_PHASE:
                {
                    if ( player.getActions() == 0 ) {
                        phase = GamePhase::BUY_PHASE;
                    }
                }
                break;
            case GamePhase::BUY_PHASE:
                {
                    if ( player.getBuys() == 0 ) {
                        end_turn();
                    }
                }
                break;
            case GamePhase::PLAYING_ACTION_CARD: // we do nothing here
                break;
            default:
                {
                    LOG(ERROR) << "tried to switch from GamePhase::" << static_cast<int>(phase)
                               << ", which does not exist";
                    throw std::runtime_error("unreachable code");
                }
        }
    }

} // namespace server
