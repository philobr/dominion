
#include <server/lobbies/lobby.h>
#include <shared/game/game_state/board_base.h>
#include <shared/utils/assert.h>
#include <shared/utils/logger.h>

namespace server
{
    void Lobby::handleMessage(MessageInterface &message_interface,
                              std::unique_ptr<shared::ClientToServerMessage> &message)
    {
        auto &msg_ref = *message;

        // NOLINTBEGIN(bugprone-macro-parentheses)
#define HANDLE(message_type, handler_func)                                                                             \
    if ( typeid(msg_ref) == typeid(shared::message_type) ) {                                                           \
        LOG(INFO) << "Trying to handle: " << #message_type;                                                            \
        std::unique_ptr<shared::message_type> casted_message(static_cast<shared::message_type *>(message.release()));  \
        handler_func(message_interface, std::move(casted_message));                                                    \
        return;                                                                                                        \
    }
        // NOLINTEND(bugprone-macro-parentheses)

        // handle messages the lobby is responsible for
        HANDLE(JoinLobbyRequestMessage, join);
        HANDLE(StartGameRequestMessage, startGame);
        // HANDLE(GameStateRequestMessage, /*gamestatereq msg*/);

        // this will get handled by the game_interface
        HANDLE(ActionDecisionMessage, receiveAction);

        LOG(ERROR) << "Lobby received unknown message type";
        throw std::runtime_error("unreachable code");
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
        if ( playerInLobby(player_id) ) {
            LOG(DEBUG) << "Player is already in the lobby. Lobby ID: " << lobby_id << " , Player ID: " << player_id;
            shared::ResultResponseMessage failure_message = shared::ResultResponseMessage(
                    lobby_id, false, request->message_id, "Player is already in the lobby");

            message_interface.sendMessage(std::make_unique<shared::ResultResponseMessage>(failure_message), player_id);
            return;
        }

        if ( players.size() >= shared::board_config::MAX_PLAYER_COUNT ) {
            LOG(DEBUG) << "Lobby is full. Lobby ID: " << lobby_id << " , Player ID: " << player_id
                       << " , Max players: " << shared::board_config::MAX_PLAYER_COUNT;
            shared::ResultResponseMessage failure_message =
                    shared::ResultResponseMessage(lobby_id, false, request->message_id, "Lobby is full");
            message_interface.sendMessage(std::make_unique<shared::ResultResponseMessage>(failure_message), player_id);
            return;
        }

        // Add player to the lobby
        players.push_back(player_id);

        // Send JoinLobbyBroadcast to all players
        for ( const auto &player_id : players ) {
            shared::JoinLobbyBroadcastMessage join_message = shared::JoinLobbyBroadcastMessage(lobby_id, players);
            message_interface.sendMessage(std::make_unique<shared::JoinLobbyBroadcastMessage>(join_message), player_id);
        }

        shared::ResultResponseMessage success_message =
                shared::ResultResponseMessage(lobby_id, true, request->message_id);

        message_interface.sendMessage(std::make_unique<shared::ResultResponseMessage>(success_message), player_id);
        return;
    };

    // PRE: selected_cards are validated in message parsing
    void Lobby::startGame(MessageInterface &message_interface, std::unique_ptr<shared::StartGameRequestMessage> request)
    {
        // Check if gamemaster is starting the game
        Player::id_t player_id = request->player_id;
        LOG(INFO) << "Lobby::start_game called with Lobby ID: " << lobby_id << " and Player ID: " << player_id;
        if ( player_id != game_master ) {
            LOG(DEBUG) << "Lobby::start_game is called by someone differnt than game master. Lobby ID: " << lobby_id
                       << " , Player ID: " << player_id << " , Game Master ID: " << game_master;
            shared::ResultResponseMessage failure_message = shared::ResultResponseMessage(
                    lobby_id, false, request->message_id, "Only the game master can start the game");
            message_interface.sendMessage(std::make_unique<shared::ResultResponseMessage>(failure_message), player_id);
            return;
        }

        if ( players.size() < shared::board_config::MIN_PLAYER_COUNT ||
             players.size() > shared::board_config::MAX_PLAYER_COUNT ) {
            LOG(DEBUG) << "Lobby::start_game is called with not enough or to many players. Lobby ID: " << lobby_id
                       << " , Player ID: " << player_id << " , Number of players: " << players.size();
            shared::ResultResponseMessage failure_message = shared::ResultResponseMessage(
                    lobby_id, false, request->message_id, "Invalid player count to start game");
            message_interface.sendMessage(std::make_unique<shared::ResultResponseMessage>(failure_message), player_id);
            return;
        }

        // Create new game interface
        game_interface = GameInterface::make(lobby_id, request->selected_cards, players);

        // get game state for game_state_message
        std::shared_ptr<GameState> game_state = game_interface->getGameState();
        if ( !game_state ) {
            LOG(ERROR) << "Game state in Lobby::start_game is nullptr";
            throw std::runtime_error("Game state is nullptr");
        }

        // send messages
        for ( const auto &p_id : players ) {
            // send start game broadcast to all players
            LOG(INFO) << "Sending StartGameBroadcastMessage in Lobby ID: " << lobby_id << " to Player ID: " << p_id;
            shared::StartGameBroadcastMessage start_message = shared::StartGameBroadcastMessage(lobby_id);
            message_interface.sendMessage(std::make_unique<shared::StartGameBroadcastMessage>(start_message), p_id);

            // send game state to all players
            LOG(INFO) << "Sending GameStateMessage in Lobby ID: " << lobby_id << " to Player ID: " << p_id;
            std::unique_ptr<reduced::GameState> reduced_game_state = game_state->getReducedState(p_id);
            std::unique_ptr<shared::GameStateMessage> game_state_message =
                    std::make_unique<shared::GameStateMessage>(lobby_id, std::move(reduced_game_state));
            message_interface.sendMessage(std::move(game_state_message), p_id);
        }
    }

    void Lobby::receiveAction(MessageInterface &message_interface,
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
        if ( !playerInLobby(player_id) ) {
            LOG(DEBUG) << "Received Action and Player is not in the requested lobby. Lobby ID: " << lobby_id
                       << " , Player ID: " << player_id << " , Message ID: " << action->message_id;
            shared::ResultResponseMessage failure_message =
                    shared::ResultResponseMessage(lobby_id, false, action->message_id, "Player is not in the lobby");
            message_interface.sendMessage(std::make_unique<shared::ResultResponseMessage>(failure_message), player_id);
            return;
        }
        GameInterface::response_t response_order =
                game_interface->receiveAction(std::move(action->decision), action->message_id, player_id);
        std::unique_ptr<shared::ActionOrderMessage> response_msg =
                std::make_unique<shared::ActionOrderMessage>(lobby_id, std::move(response_order));
        message_interface.sendMessage(std::move(response_msg), player_id);
    }
} // namespace server
