
#include <server/game_state.h>
#include <shared/game_state.h>
#include <shared/utils/assert.h>

namespace server
{
    void GameState::add_player(Player player) { players.push_back(Player(player)); }

    shared::ReducedGameState GameState::get_reduced_state(shared::PlayerBase::id_t player_id)
    {
        throw std::runtime_error("Not implemented yet");
    }

    const std::vector<Player> &GameState::get_players() { return players; }
} // namespace server
