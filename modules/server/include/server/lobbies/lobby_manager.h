
#pragma once

#include <map>
#include <memory>
#include <string>

#include <shared/message_types.h>
#include <server/lobbies/lobby.h>
#include <server/network/message_interface.h>
#include <shared/game/game_state/reduced_game_state.h>

namespace server{
    /**
     * @brief The lobby manager contains all the running games / lobbies.
     *
     * The lobby manager is responsible for creating, joining and starting games.
     * It also receives actions from players and passes them on to the correct game.
     */
    class LobbyManager
    {
    public:
        // TODO: The message interface should not be passed to the constructor, but to the methods that need it.
        // Also, the message interface should definitely not be passed as a raw pointer.
        LobbyManager(std::shared_ptr<MessageInterface> message_interface) : message_interface(message_interface){};
        /*message_interface(std::make_unique<MessageInterface>(message_interface))*/

        void create_lobby(std::unique_ptr<shared::CreateLobbyRequestMessage> request);
        void join_lobby(std::unique_ptr<shared::JoinLobbyRequestMessage> request);
        void start_game(std::unique_ptr<shared::StartGameRequestMessage> request);
        void receive_action(std::unique_ptr<shared::ActionDecisionMessage> action);
        shared::ReducedGameState get_game_state(std::string game_id, shared::PlayerBase::id_t player);
        const std::map<std::string, std::shared_ptr<Lobby>> *get_games() { return &games; };

    private:
        std::map<std::string, std::shared_ptr<Lobby>> games;
        std::shared_ptr<MessageInterface> message_interface;
    };
} // namespace server