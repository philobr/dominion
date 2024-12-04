#include <algorithm>
#include <random>

#include <server/game/game_state.h>

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
                                                    std::move(reduced_enemies), active_player_id);
    }

    void GameState::endTurn()
    {
        switchPlayer();

        maybeSwitchPhase(); // a player might not have any action cards at the beginning of the action phase

        if ( isGameOver() ) {
            endGame();
        }
    }

    bool GameState::isGameOver() const { return board->isGameOver(); }

    bool GameState::validateKingdomCardTypes(const std::vector<shared::CardBase::id_t> &kingdom_cards)
    {
        return std::all_of(kingdom_cards.begin(), kingdom_cards.end(),
                           [](const auto &card_id)
                           {
                               const auto &card = shared::CardFactory::getCard(card_id);
                               return card.isAction() || card.isAttack() || card.isReaction();
                           });
    }

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

    void GameState::maybeSwitchPhase()
    {
        auto &player = getCurrentPlayer();
        switch ( phase ) {
            case GamePhase::ACTION_PHASE:
                {
                    if ( player.getActions() == 0 ||
                         !player.hasType<shared::CardAccess::HAND>(shared::CardType::ACTION) ) {
                        phase = GamePhase::BUY_PHASE;
                    }
                }
                break;
            case GamePhase::BUY_PHASE:
                {
                    if ( player.getBuys() == 0 ) {
                        endTurn();
                    }
                }
                break;
            case GamePhase::PLAYING_ACTION_CARD:
            default:
                {
                    LOG(ERROR) << "Invalid game phase: " << static_cast<int>(phase);
                    throw std::runtime_error("Unreachable code.");
                }
        }
    }

    void GameState::tryEndActionPhase(const shared::PlayerBase::id_t &requestor_id)
    {
        if ( requestor_id != getCurrentPlayerId() ) {
            LOG(WARN) << "Player " << requestor_id << " attempted to end the action phase out of turn.";
            throw exception::InvalidRequest("Not your turn.");
        }

        if ( phase != GamePhase::ACTION_PHASE ) {
            LOG(WARN) << "Player " << requestor_id << " attempted to end the action phase while in phase "
                      << toString(phase);
            throw exception::OutOfPhase("Cannot end action phase while in " + toString(phase));
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
            LOG(WARN) << "Player " << requestor_id << " attempted to buy a card during " << toString(phase);
            throw exception::OutOfPhase("Cannot buy cards while in " + toString(phase));
        }

        auto player = getPlayer(requestor_id);
        const auto card_cost = shared::CardFactory::getCard(card_id).getCost();

        if ( !player.canBuy(card_cost) ) {
            LOG(WARN) << "Player " << requestor_id << " cannot afford card " << card_id << " (cost: " << card_cost
                      << ", treasure: " << player.getTreasure() << ", buys: " << player.getBuys() << ").";
            throw exception::InsufficientFunds();
        }

        board->tryTake(card_id);

        player.decTreasure(card_cost);
        player.decBuys();
        player.gain(card_id);

        LOG(INFO) << "Player " << requestor_id << " successfully bought card " << card_id;
    }

    void GameState::tryGain(const shared::PlayerBase::id_t &requestor_id, const shared::CardBase::id_t &card_id)
    {
        if ( phase != GamePhase::PLAYING_ACTION_CARD ) {
            LOG(WARN) << "Player " << requestor_id << " attempted to gain a card outside of the action card phase.";
            throw exception::OutOfPhase("Cannot gain a card while in " + toString(phase));
        }

        board->tryTake(card_id);

        auto player = getPlayer(requestor_id);
        player.gain(card_id);

        LOG(INFO) << "Player " << requestor_id << " successfully gained card " << card_id;
    }

    void GameState::tryPlayFromHand(const shared::PlayerBase::id_t &requestor_id, const shared::CardBase::id_t &card_id)
    {
        if ( requestor_id != getCurrentPlayerId() ) {
            LOG(WARN) << "Player " << requestor_id << " attempted to play a card out of turn.";
            throw exception::InvalidRequest("Not your turn.");
        }

        if ( phase != GamePhase::ACTION_PHASE ) {
            LOG(WARN) << "Player " << requestor_id << " attempted to play card " << card_id << " during "
                      << toString(phase);
            throw exception::OutOfPhase("Cannot play cards while in " + toString(phase));
        }

        auto player = getPlayer(requestor_id);

        if ( player.getActions() == 0 ) {
            LOG(WARN) << "Player " << requestor_id << " attempted to play card " << card_id << " with no actions left.";
            throw exception::OutOfActions();
        }

        if ( !player.hasCard<shared::CardAccess::HAND>(card_id) ) {
            LOG(WARN) << "Player " << requestor_id << " attempted to play card " << card_id << " not in their hand.";
            throw exception::CardNotAvailable();
        }

        player.playCardFromHand(card_id);
        board->addToPlayedCards(card_id);
        player.decActions();

        LOG(INFO) << "Player " << requestor_id << " successfully played card " << card_id << " from their hand.";
    }

    void GameState::tryPlayFromStaged(const shared::PlayerBase::id_t &requestor_id,
                                      const shared::CardBase::id_t &card_id)
    {
        if ( requestor_id != getCurrentPlayerId() ) {
            LOG(WARN) << "Player " << requestor_id << " attempted to play a staged card out of turn.";
            throw exception::InvalidRequest("Not your turn.");
        }

        if ( phase != GamePhase::PLAYING_ACTION_CARD ) {
            LOG(WARN) << "Player " << requestor_id << " attempted to play staged card " << card_id << " during "
                      << toString(phase);
            throw exception::OutOfPhase("Cannot play staged cards while in " + toString(phase));
        }

        auto player = getPlayer(requestor_id);
        if ( !player.hasCard<shared::CardAccess::STAGED_CARDS>(card_id) ) {
            LOG(WARN) << "Player " << requestor_id << " attempted to play card " << card_id << " not in staged cards.";
            throw exception::CardNotAvailable();
        }

        player.playCardFromStaged(card_id);
        board->addToPlayedCards(card_id);

        LOG(INFO) << "Player " << requestor_id << " successfully played staged card " << card_id;
    }

} // namespace server
