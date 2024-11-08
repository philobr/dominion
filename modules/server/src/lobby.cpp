#include <server/lobbies.h>
#include <shared/game_state.h>

server::Lobby::Lobby(shared::PlayerBase::id_t game_master) : game_state(GameState()), game_master(game_master)
{
    game_state.add_player(Player(game_master));
};

void server::Lobby::join(MessageInterface &message_interface, shared::JoinLobbyRequestMessage request)
{
    shared::PlayerBase::id_t player_id = request.player_id;
    // Check if player is already in the lobby
    for ( Player player : game_state.get_players() ) {
        if ( player.getId() == player_id ) {
            shared::ResultResponseMessage failure_message =
                    shared::ResultResponseMessage(false, request.message_id, "Player is already in the lobby");
            message_interface.send_message(&failure_message, player_id);
            return;
        }
    }
    // Send JoinLobbyBroadcast to all players
    for ( Player player : game_state.get_players() ) {
        shared::JoinLobbyBroadcastMessage join_message = shared::JoinLobbyBroadcastMessage(player_id);
        message_interface.send_message(&join_message, player.getId());
    }
    // Add player to the lobby
    game_state.add_player(Player(player_id));

    shared::ResultResponseMessage success_message = shared::ResultResponseMessage(true, request.message_id);
    message_interface.send_message(&success_message, player_id);
    return;
};

// PRE: selected_cards are validated in message parsing
void server::Lobby::start_game(MessageInterface &message_interface, shared::StartGameRequestMessage request)
{
    // Check if gamemaster is starting the game
    shared::PlayerBase::id_t requestor_id = request.player_id;
    if ( requestor_id != game_master ) {
        shared::ResultResponseMessage failure_message =
                shared::ResultResponseMessage(false, request.message_id, "Only the game master can start the game");
        message_interface.send_message(&failure_message, requestor_id);
        return;
    }
    // set board cards
    game_state.start_game(request.selected_cards);
    // send game state to all players
    for ( Player player : game_state.get_players() ) {
        shared::StartGameBroadcastMessage start_message = shared::StartGameBroadcastMessage();
        message_interface.send_message(&start_message, player.getId());
        shared::GameStateMessage game_state_message = shared::GameStateMessage();
        message_interface.send_message(&game_state_message, player.getId());
    }
    return;
};