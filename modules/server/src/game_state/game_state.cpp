#include <server/game/game_state/game_state.h>
#include <shared/utils/assert.h>

#include <algorithm>
#include <random>

namespace server
{
    void GameState::receive_action(std::unique_ptr<shared::ActionDecision> action, MessageInterface &message_interface)
    {
        /*
        if ( auto play_action = dynamic_cast<shared::PlayActionCardDecision *>(action.get()) ) {
            // Handle PlayActionCardDecision
            // You can access play_action->cardIndex here
        } else if ( auto buy_action = dynamic_cast<shared::BuyCardDecision *>(action.get()) ) {
            // Handle BuyCardDecision
            // You can access buy_action->card here
            shared::CardBase::id_t card = buy_action->card;
            if ( try_buy(get_current_player_id(), card) ) {
                // Notify all players about the bought card
                // TODO: There are no messages for this yet
            } else {
                // Notify the player that the card could not be bought
                // TODO: There are no messages for this yet
            }
        } else if ( auto end_turn = dynamic_cast<shared::EndTurnDecision *>(action.get()) ) {
            // Handle EndTurnDecision
            this->end_turn();
        } else if ( auto choose_cards = dynamic_cast<shared::ChooseNCardsFromHandDecision *>(action.get()) ) {
            // Handle ChooseNCardsFromHandDecision
            // You can access choose_cards->cards here
        } else {
            // Handle unknown action type
            throw std::runtime_error("Unknown action type received");
        }
        */
    }
} // namespace server


// OKISH

namespace server
{
    GameState::GameState(const std::vector<shared::CardBase::id_t> &play_cards,
                         const std::vector<Player::id_t> &player_ids) : current_player_idx(0)
    {
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
            board = std::make_unique<ServerBoard>(*other.board);
        }
    }

    void GameState::initialise_players(const std::vector<Player::id_t> &player_ids)
    {
        player_order = player_ids; // for now the player order will be the same as the list of player ids
        for ( auto &id : player_ids ) {
            if ( player_map.contains(id) ) {
                throw std::runtime_error("cant add the same player twice!");
            }

            player_map[id] = std::make_unique<Player>(id);

            for ( unsigned i = 0; i < 7; i++ ) {
                if ( i < 3 ) {
                    player_map[id]->add_to_discard_pile("Estate");
                }
                player_map[id]->add_to_discard_pile("Copper");
            }

            player_map[id]->draw(5);
        }
    }

    void GameState::initialise_board(const std::vector<shared::CardBase::id_t> &selected_cards)
    {
        _ASSERT_EQ(selected_cards.size(), size_t(10), "the game must be played with 10 kingdom cards!");
        board = std::make_unique<server::ServerBoard>(selected_cards, player_map.size());
    }

    shared::ReducedGameState GameState::get_reduced_state(const Player::id_t &target_player)
    {
        shared::ReducedPlayer reduced_player;
        std::vector<shared::ReducedEnemy> reduced_enemies;

        std::for_each(player_map.begin(), player_map.end(),
                      [&](auto &entry)
                      {
                          auto &[player_id, player_ptr] = entry;

                          if ( player_id == target_player ) {
                              reduced_player = player_ptr->get_reduced_player();
                          } else {
                              reduced_enemies.push_back(player_ptr->get_reduced_enemy());
                          }
                      });

        Player::id_t active_player_id = get_current_player_id();
        shared::Board reduced_board = *board; // TODO:

        return shared::ReducedGameState(reduced_board, reduced_player, reduced_enemies, active_player_id);
    }

    bool GameState::try_buy(const Player::id_t player_id, const shared::CardBase::id_t &card)
    {
        if ( !board->buy(card) ) {
            return false;
        }

        auto &player = get_player(player_id);
        player.add(player.get_discard_pile(), card);
        return true;
    }

    void GameState::end_turn()
    {
        get_current_player().end_turn();
        switch_player();

        if ( is_game_over() ) {
            end_game();
        }
    }

    bool GameState::is_game_over() const { return board->isGameOver(); }

} // namespace server
