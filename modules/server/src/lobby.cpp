
#include <server/lobbies/lobby.h>
#include <shared/utils/assert.h>
#include <shared/utils/logger.h>

const unsigned int MAX_PLAYERS = 4;
namespace server
{
    bool Lobby::player_in_lobby(const Player::id_t &player_id)
    {
        return std::any_of(players.begin(), players.end(), [&](const auto &player) { return player == player_id; });
    }

    Lobby::Lobby(const Player::id_t &game_master, const std::string &lobby_id) :
        game_interface(nullptr), game_master(game_master), lobby_id(lobby_id)
    {
        LOG(INFO) << "Lobby constructor called with lobby_id: " << lobby_id;
        players.push_back(game_master);
    };

    void Lobby::join(MessageInterface &message_interface, std::unique_ptr<shared::JoinLobbyRequestMessage> request)
    {
        const Player::id_t player_id = request->player_id;
        LOG(INFO) << "Lobby::join called with Lobby ID: " << lobby_id << " and Player ID: " << player_id;
        // Check if player is already in the lobby
        if ( player_in_lobby(player_id) ) {
            LOG(DEBUG) << "Player is already in the lobby. Lobby ID: " << lobby_id << " , Player ID: " << player_id;
            shared::ResultResponseMessage failure_message = shared::ResultResponseMessage(
                    lobby_id, false, request->message_id, "Player is already in the lobby");

            message_interface.send_message(std::make_unique<shared::ResultResponseMessage>(failure_message), player_id);
            return;
        }

        if ( players.size() >= MAX_PLAYERS ) {
            LOG(DEBUG) << "Lobby is full. Lobby ID: " << lobby_id << " , Player ID: " << player_id
                       << " , Max players: " << MAX_PLAYERS;
            shared::ResultResponseMessage failure_message =
                    shared::ResultResponseMessage(lobby_id, false, request->message_id, "Lobby is full");
            message_interface.send_message(std::make_unique<shared::ResultResponseMessage>(failure_message), player_id);
            return;
        }

        // Add player to the lobby
        players.push_back(player_id);

        // Send JoinLobbyBroadcast to all players
        for ( const auto &player_id : players ) {
            shared::JoinLobbyBroadcastMessage join_message = shared::JoinLobbyBroadcastMessage(lobby_id, players);
            message_interface.send_message(std::make_unique<shared::JoinLobbyBroadcastMessage>(join_message),
                                           player_id);
        }

        shared::ResultResponseMessage success_message =
                shared::ResultResponseMessage(lobby_id, true, request->message_id);

        message_interface.send_message(std::make_unique<shared::ResultResponseMessage>(success_message), player_id);
        return;
    };

    // PRE: selected_cards are validated in message parsing
    void Lobby::start_game(MessageInterface &message_interface,
                           std::unique_ptr<shared::StartGameRequestMessage> request)
    {
        // Check if gamemaster is starting the game
        Player::id_t player_id = request->player_id;
        LOG(INFO) << "Lobby::start_game called with Lobby ID: " << lobby_id << " and Player ID: " << player_id;
        if ( player_id != game_master ) {
            LOG(DEBUG) << "Lobby::start_game is called by someone differnt than game master. Lobby ID: " << lobby_id
                       << " , Player ID: " << player_id << " , Game Master ID: " << game_master;
            shared::ResultResponseMessage failure_message = shared::ResultResponseMessage(
                    lobby_id, false, request->message_id, "Only the game master can start the game");
            message_interface.send_message(std::make_unique<shared::ResultResponseMessage>(failure_message), player_id);
            return;
        }

        if ( players.size() < 2 ) {
            LOG(DEBUG) << "Lobby::start_game is called with less than 2 players. Lobby ID: " << lobby_id
                       << " , Player ID: " << player_id << " , Number of players: " << players.size();
            shared::ResultResponseMessage failure_message = shared::ResultResponseMessage(
                    lobby_id, false, request->message_id, "Not enough players to start game");
            message_interface.send_message(std::make_unique<shared::ResultResponseMessage>(failure_message), player_id);
            return;
        }

        if ( players.size() > MAX_PLAYERS ) {
            LOG(ERROR) << "Tried starting a game with more than MAX_PLAYERS: " << MAX_PLAYERS
                       << " players. Lobby ID: " << lobby_id;
            throw std::runtime_error("Tried starting a game with more than MAX_PLAYERS players");
        }

        // Create new game interface
        game_interface = GameInterface::make(lobby_id, request->selected_cards, players);

        // get game state for game_state_message
        std::shared_ptr<GameState> game_state = game_interface->get_game_state();
        if ( !game_state ) {
            LOG(ERROR) << "Game state in Lobby::start_game is nullptr";
            throw std::runtime_error("Game state is nullptr");
        }

        // send messages
        for ( const auto &p_id : players ) {
            // send start game broadcast to all players
            LOG(INFO) << "Sending StartGameBroadcastMessage in Lobby ID: " << lobby_id << " to Player ID: " << p_id;
            shared::StartGameBroadcastMessage start_message = shared::StartGameBroadcastMessage(lobby_id);
            message_interface.send_message(std::make_unique<shared::StartGameBroadcastMessage>(start_message), p_id);

            // send game state to all players
            LOG(INFO) << "Sending GameStateMessage in Lobby ID: " << lobby_id << " to Player ID: " << p_id;
            std::unique_ptr<reduced::GameState> reduced_game_state = game_state->get_reduced_state(p_id);
            std::unique_ptr<shared::GameStateMessage> game_state_message =
                    std::make_unique<shared::GameStateMessage>(lobby_id, std::move(reduced_game_state));
            message_interface.send_message(std::move(game_state_message), p_id);
        }
    }

    void Lobby::receive_action(MessageInterface &message_interface,
                               std::unique_ptr<shared::ActionDecisionMessage> action)
    {
        LOG(INFO) << "Lobby::receive_action called with Lobby ID: " << lobby_id
                  << " and Player ID: " << action->player_id;
        // Check if game has started
        if ( !game_interface ) {
            LOG(ERROR) << "Game interface in Lobby::receive_action is nullptr. The game hasn't started yet.";
            throw std::runtime_error("Game interface is nullptr");
        }

        // Check if player is in the lobby
        Player::id_t player_id = action->player_id;
        if ( !player_in_lobby(player_id) ) {
            LOG(DEBUG) << "Received Action and Player is not in the requested lobby. Lobby ID: " << lobby_id
                       << " , Player ID: " << player_id << " , Message ID: " << action->message_id;
            shared::ResultResponseMessage failure_message =
                    shared::ResultResponseMessage(lobby_id, false, action->message_id, "Player is not in the lobby");
            message_interface.send_message(std::make_unique<shared::ResultResponseMessage>(failure_message), player_id);
            return;
        }
        GameInterface::response_t response_order =
                game_interface->receive_action(std::move(action->decision), action->message_id, player_id);
        std::unique_ptr<shared::ActionOrderMessage> response_msg =
                std::make_unique<shared::ActionOrderMessage>(lobby_id, std::move(response_order));
        message_interface.send_message(std::move(response_msg), player_id);
    }
} // namespace server
