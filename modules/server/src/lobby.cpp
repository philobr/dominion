#include <server/lobbies.h>
#include <shared/game_state.h>

server::Lobby::Lobby(shared::PlayerBase::id_t game_master) : game_state(GameState()), game_master(game_master) {
    game_state.add_player(Player(game_master));
};

void server::Lobby::join(MessageInterface message_interface, shared::JoinLobbyRequestMessage request){
    shared::PlayerBase::id_t player_id = request.player_id;
    // Check if player is already in the lobby
    for (Player player : game_state.get_players()){
        if (player.getId() == player_id){
            message_interface.send_message(shared::ResultResponseMessage(false, request.message_id, "Player is already in the lobby"), player_id);
            return;
        }
    }
    // Add player to the lobby
    game_state.add_player(Player(player_id));
    // Send game state to all players
    for (Player player : game_state.get_players()){
        message_interface.send_message(shared::JoinLobbyBroadcast(player_id), player.getId());
    }
    message_interface.send_message(shared::ResultResponseMessage(true, request.message_id), player_id);
    return;
};

// PRE: selected_cards are validated in message parsing
void server::Lobby::start_game(MessageInterface message_interface, shared::StartGameRequestMessage request){
    // Check if gamemaster is starting the game
    shared::PlayerBase::id_t requestor_id = request.player_id;
    if (requestor_id != game_master){
        message_interface.send_message(shared::ResultResponseMessage(false, request.message_id, "Only the game master can start the game"), requestor_id);
        return;
    }
    // set board cards
    game_state.start_game(request.get_selected_cards());
    // send game state to all players
    for (Player player : game_state.get_players()){
        message_interface.send_message(shared::StartGameResponseMessage(), player.getId());
        message_interface.send_game_state(game_state, player.getId());
    }
    return;
};