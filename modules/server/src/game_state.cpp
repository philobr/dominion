#include <server/game_state.h>

void server::GameState::add_player(Player player) { players.push_back(Player(player)); }

const std::vector<server::Player> &server::GameState::get_players() { return players; }