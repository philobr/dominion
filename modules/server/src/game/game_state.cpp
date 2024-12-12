#include <algorithm>
#include <random>

#include <server/game/game_state.h>

#include <shared/game/cards/card_factory.h>
#include <shared/utils/assert.h>
#include <shared/utils/exception.h>
#include <shared/utils/logger.h>

using shared::GamePhase;

namespace server
{
    GameState::GameState(const std::vector<shared::CardBase::id_t> &play_cards,
                         const std::vector<Player::id_t> &player_ids) :
        current_player_idx(0),
        phase(GamePhase::ACTION_PHASE)
    {
        if ( player_ids.size() < 2 || player_ids.size() > 4 ) {
            LOG(ERROR) << "Invalid number of players: expected 2-4, got " << player_ids.size();
            throw exception::PlayerCountMismatch("Invalid player count!");
        }

        initialisePlayers(player_ids);
        initialiseBoard(play_cards);
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

    std::vector<shared::PlayerResult> GameState::getResults() const
    {
        std::vector<shared::PlayerResult> results;
        // Get results of each player
        for ( const auto &pair : player_map ) {
            const auto &player = pair.second;
            int victory_points = player->getVictoryPoints();
            shared::PlayerResult result(player->getId(), victory_points);
            results.emplace_back(result);
        }
        // and sort them by score
        std::sort(results.begin(), results.end(),
                  [](const auto &lhs, const auto &rhs) { return lhs.score() > rhs.score(); });
        return results;
    }

    void GameState::initialisePlayers(const std::vector<Player::id_t> &player_ids)
    {
        player_order = player_ids;
        for ( const auto &id : player_ids ) {
            if ( player_map.count(id) != 0u ) {
                LOG(ERROR) << "Duplicate player ID: " << id;
                throw exception::DuplicatePlayer("Cannot add the same player twice!");
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

    void GameState::initialiseBoard(const std::vector<shared::CardBase::id_t> &selected_cards)
    {
        if ( selected_cards.size() != size_t(10) ) {
            LOG(ERROR) << "Invalid number of kingdom cards: expected 10, got " << selected_cards.size();
            throw exception::WrongCardCount("Incorrect number of kingdom cards!");
        }
        board = server::ServerBoard::make(selected_cards, player_map.size());
    }

    std::unique_ptr<reduced::GameState> GameState::getReducedState(const Player::id_t &target_player)
    {
        std::vector<reduced::Enemy::ptr_t> reduced_enemies;
        std::for_each(player_map.begin(), player_map.end(),
                      [&](auto &entry)
                      {
                          if ( auto &[player_id, player_ptr] = entry; player_id != target_player ) {
                              reduced_enemies.emplace_back(player_ptr->getReducedEnemy());
                          }
                      });

        auto reduced_player = getPlayer(target_player).getReducedPlayer();
        Player::id_t active_player_id = getCurrentPlayerId();
        shared::Board::ptr_t reduced_board = board->getReduced();

        return std::make_unique<reduced::GameState>(reduced_board, std::move(reduced_player),
                                                    std::move(reduced_enemies), active_player_id, phase);
    }

    void GameState::endTurn()
    {
        getCurrentPlayer().endTurn();
        switchPlayer();
        resetPhase();
        board->resetPlayedCards();

        bool turn_ended =
                maybeSwitchPhase(); // a player might not have any action cards at the beginning of the action phase
        if ( turn_ended ) {
            throw std::runtime_error("Tried to end turn twice, this should never happen.");
        }
    }

    bool GameState::isGameOver() const { return board->isGameOver(); }

    void GameState::forceSwitchPhase()
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
                    LOG(WARN) << "Cannot switch phase: still playing an action card.";
                    throw exception::OutOfPhase("Must finish playing the action card first.");
                }
            default:
                {
                    LOG(ERROR) << "Invalid game phase: " << static_cast<int>(phase);
                    throw std::runtime_error("Unreachable code.");
                }
        }
    }

    bool GameState::maybeSwitchPhase()
    {
        switch ( phase ) {
            case GamePhase::ACTION_PHASE:
                {
                    if ( getCurrentPlayer().getActions() == 0 ||
                         !getCurrentPlayer().hasType<shared::CardAccess::HAND>(shared::CardType::ACTION) ) {
                        phase = GamePhase::BUY_PHASE;
                    }
                    return false;
                }
            case GamePhase::BUY_PHASE:
                {
                    if ( getCurrentPlayer().getBuys() == 0 ) {
                        endTurn();
                        return true;
                    } else {
                        return false;
                    }
                }
            case GamePhase::PLAYING_ACTION_CARD:
            default:
                {
                    LOG(ERROR) << "Invalid game phase: " << static_cast<int>(phase);
                    throw std::runtime_error("Invalid game phase in maybeSwitchPhase");
                }
        }
    }

    std::vector<shared::CardBase::id_t> GameState::playAllTreasures(const shared::PlayerBase::id_t &affected_player_id)
    {
        auto &player = getPlayer(affected_player_id);

        if ( !player.hasType<shared::HAND>(shared::CardType::TREASURE) ) {
            LOG(INFO) << "Player: \'" << player.getId() << "\' has no treasures, returning nothing";
            return {};
        }

        auto treasure_cards = player.getType<shared::CardAccess::HAND>(shared::CardType::TREASURE);
        player.move<shared::HAND, shared::PLAYED_CARDS>(treasure_cards);
        board->addToPlayedCards(treasure_cards);
        return treasure_cards;
    }

    void GameState::tryEndActionPhase(const shared::PlayerBase::id_t &requestor_id)
    {
        if ( requestor_id != getCurrentPlayerId() ) {
            LOG(WARN) << "Player " << requestor_id << " attempted to end the action phase out of turn.";
            throw exception::InvalidRequest("Not your turn.");
        }

        if ( phase != GamePhase::ACTION_PHASE ) {
            LOG(WARN) << "Player " << requestor_id << " attempted to end the action phase while in phase "
                      << gamePhaseToString(phase);
            throw exception::OutOfPhase("Cannot end action phase while in " + gamePhaseToString(phase));
        }

        forceSwitchPhase();
    }

    void GameState::tryBuy(const shared::PlayerBase::id_t &requestor_id, const shared::CardBase::id_t &card_id)
    {
        if ( requestor_id != getCurrentPlayerId() ) {
            LOG(WARN) << "Player " << requestor_id << " attempted to buy a card out of turn.";
            throw exception::InvalidRequest("Not your turn.");
        }

        if ( phase != GamePhase::BUY_PHASE ) {
            LOG(WARN) << "Player " << requestor_id << " attempted to buy a card during " << gamePhaseToString(phase);
            throw exception::OutOfPhase("Cannot buy cards while in " + gamePhaseToString(phase));
        }
        const auto card_cost = shared::CardFactory::getCard(card_id).getCost();

        if ( !getPlayer(requestor_id).canBuy(card_cost) ) {
            LOG(WARN) << "Player " << requestor_id << " cannot afford card " << card_id << " (cost: " << card_cost
                      << ", treasure: " << getPlayer(requestor_id).getTreasure()
                      << ", buys: " << getPlayer(requestor_id).getBuys() << ").";
            throw exception::InsufficientFunds();
        }

        board->tryTake(card_id);

        getPlayer(requestor_id).decTreasure(card_cost);
        getPlayer(requestor_id).decBuys();
        getPlayer(requestor_id).gain(card_id);

        LOG(INFO) << "Player " << requestor_id << " successfully bought card " << card_id;
    }

} // namespace server
