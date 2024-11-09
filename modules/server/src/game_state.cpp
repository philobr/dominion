
#include <server/game_state.h>
#include <shared/game_state.h>
#include <shared/utils/assert.h>

namespace server
{
    void GameState::add_player(Player player) { players.push_back(Player(player)); }

    shared::ReducedGameState GameState::get_reduced_state(shared::PlayerBase::id_t player_id)
    {
        shared::ReducedGameState *reduced_state = nullptr;
        ASSERT_NEQ(reduced_state, (shared::ReducedGameState *)nullptr, "Not implemented yet");
        return *reduced_state;
    }

    const std::vector<Player> &GameState::get_players() { return players; }
} // namespace server
