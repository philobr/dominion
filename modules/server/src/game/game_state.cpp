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
        if ( player_ids.size() < 2 && player_ids.size() > 4 ) {
            LOG(ERROR) << "Should have 2-4 players, but got: " << player_ids.size();
            throw exception::PlayerCountMismatch("wrong player count!");
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

    void GameState::initialiseBoard(const std::vector<shared::CardBase::id_t> &selected_cards)
    {
        if ( selected_cards.size() != size_t(10) ) {
            LOG(ERROR) << "Excepcted 10 cards but got " << selected_cards.size();
            throw exception::WrongCardCount("");
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
        getCurrentPlayer().endTurn();
        switchPlayer();
        resetPhase();

        board->getPlayedCards().clear();

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

    void GameState::maybeSwitchPhase()
    {
        auto &player = getCurrentPlayer();
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
                        endTurn();
                    }
                }
                break;
            case GamePhase::PLAYING_ACTION_CARD:
            default:
                {
                    LOG(ERROR) << "tried to switch from GamePhase::" << static_cast<int>(phase)
                               << ", which does not exist";
                    throw std::runtime_error("unreachable code");
                }
        }
    }

    void GameState::tryEndActionPhase(const shared::PlayerBase::id_t &requestor_id)
    {
        if ( requestor_id != getCurrentPlayerId() ) {
            LOG(WARN) << requestor_id << " tried to end ActionPhase, but he is currently not playing";
            throw exception::InvalidRequest("It's not your turn!");
        }

        if ( phase != GamePhase::ACTION_PHASE ) {
            LOG(WARN) << requestor_id << " tried to end ActionPhase, but he is currently not playing";
            throw exception::OutOfPhase("Can't end " + toString(GamePhase::ACTION_PHASE) + " while beeing in " +
                                        toString(phase));
        }

        forceSwitchPhase();
    }

    void GameState::tryBuy(const shared::PlayerBase::id_t &requestor_id, const shared::CardBase::id_t &card_id)
    {
        if ( requestor_id != getCurrentPlayerId() ) {
            LOG(WARN) << requestor_id << " tries to buy a card but he is currently not playing";
            throw exception::InvalidRequest("It's not your turn!");
        }

        if ( phase != GamePhase::BUY_PHASE ) {
            LOG(WARN) << requestor_id << " buys a card, but he is not in " << toString(GamePhase::BUY_PHASE);
            throw exception::OutOfPhase("Can't buys a card " + toString(GamePhase::BUY_PHASE) + " while beeing in " +
                                        toString(phase));
        }

        auto player = getPlayer(requestor_id);
        const auto card_cost = shared::CardFactory::getCard(card_id).getCost();

        if ( !player.canBuy(card_cost) ) {
            LOG(WARN) << requestor_id << " can not afford the card " << card_id << " (costs: " << card_cost
                      << ", has: " << player.getTreasure()
                      << "), or he is out of buys (needs: 1, has: " << player.getBuys() << ")";
            throw exception::InsufficientFunds();
        }

        board->tryTake(card_id);

        player.decTreasure(card_cost);
        player.decBuys();
        player.gain(card_id);

        LOG(INFO) << "player: " << requestor_id << " successfully bought a new card with id: " << card_id;
    }

    void GameState::tryGain(const shared::PlayerBase::id_t &requestor_id, const shared::CardBase::id_t &card_id)
    {
        if ( phase != GamePhase::PLAYING_ACTION_CARD ) {
            LOG(WARN) << requestor_id << " tries to gain a card, but he is not in "
                      << toString(GamePhase::PLAYING_ACTION_CARD);
            throw exception::OutOfPhase("Can't gain a card while beeing in " + toString(phase));
        }

        board->tryTake(card_id);

        auto player = getPlayer(requestor_id);
        player.gain(card_id);

        LOG(INFO) << "player: " << requestor_id << " successfully gained a new card with id: " << card_id;
    }

    void GameState::tryPlayFromHand(const shared::PlayerBase::id_t &requestor_id, const shared::CardBase::id_t &card_id)
    {
        if ( requestor_id != getCurrentPlayerId() ) {
            LOG(WARN) << requestor_id << " tried to call " << FUNC_NAME << ", but he is currently not playing!";
            throw exception::InvalidRequest("It's not your turn!");
        }

        if ( phase != GamePhase::ACTION_PHASE ) {
            LOG(WARN) << requestor_id << " tried to play a card with id: " << card_id
                      << " from his hand, but he is not in " << toString(GamePhase::ACTION_PHASE);
            throw exception::OutOfPhase("Can't play a card while beeing in " + toString(phase));
        }

        auto player = getPlayer(requestor_id);

        if ( player.getActions() == 0 ) {
            LOG(WARN) << requestor_id << " tried to play card: " << card_id
                      << " from his hand cards, but he has no actions left";
            throw exception::OutOfActions();
        }

        if ( !player.hasCardInHand(card_id) ) {
            LOG(WARN) << requestor_id << " tried to play card: " << card_id
                      << " from his hand cards, but he does not have it";
            throw exception::CardNotAvailable();
        }

        player.playCardFromHand(card_id);
        board->addToPlayedCards(card_id);

        LOG(INFO) << "player: " << requestor_id << " successfully played a card with id: " << card_id
                  << " from his hand";
    }

    void GameState::tryPlayFromStaged(const shared::PlayerBase::id_t &requestor_id,
                                      const shared::CardBase::id_t &card_id)
    {
        if ( requestor_id != getCurrentPlayerId() ) {
            LOG(WARN) << requestor_id << " tried to call " << FUNC_NAME << ", but he is currently not playing!";
            throw exception::InvalidRequest("It's not your turn!");
        }

        if ( phase != GamePhase::PLAYING_ACTION_CARD ) {
            LOG(WARN) << requestor_id << " tried to play a card with id: " << card_id
                      << " from his staged cards, but he is not in " << toString(GamePhase::PLAYING_ACTION_CARD);
            throw exception::OutOfPhase("Can't play a card from staged cards while beeing in " + toString(phase));
        }

        auto player = getPlayer(requestor_id);
        if ( !player.hasCardStaged(card_id) ) {
            LOG(WARN) << requestor_id << " tried to play card: " << card_id
                      << " from his staged cards, but he does not have it";
            throw exception::CardNotAvailable();
        }

        player.playCardFromStaged(card_id);
        board->addToPlayedCards(card_id);

        LOG(INFO) << "player: " << requestor_id << " successfully played a card with id: " << card_id
                  << " from his staged cards";
    }

} // namespace server
