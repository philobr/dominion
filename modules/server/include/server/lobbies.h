#pragma once

#include <map>
#include <memory>
#include <string>

#include <shared/game_state.h>
#include <shared/message_types.h>

#include <server/game_state.h>
#include <server/message_interface.h>

namespace server
{

    /**
     * @brief A lobby is a container for a game that is being played.
     *
     * A lobby is created by a game master, who is the first player to join the lobby.
     * The game master can start the game when they want to.
     */
    class Lobby
    {
    public:
        /**
         * @brief Create a new game lobby.
         *
         * @param game_master The player who created the lobby.
         */
        Lobby(shared::PlayerBase::id_t game_master);
        /**
         * @brief Add a player to the lobby.
         */
        void join(MessageInterface message_interface, shared::JoinLobbyRequestMessage request);
        void start_game(MessageInterface message_interface, shared::StartGameRequestMessage request);
        void receive_action(std::unique_ptr<shared::ActionDecisionMessage> action, MessageInterface &message_interface);
        shared::ReducedGameState get_game_state(shared::PlayerBase::id_t player) const;
        server::GameState get_full_game_state() const { return game_state; };
        shared::PlayerBase::id_t get_game_master() const { return game_master; };

    private:
        GameState game_state;
        shared::PlayerBase::id_t game_master;
    };

    /**
     * @brief The lobby manager contains all the running games / lobbies.
     *
     * The lobby manager is responsible for creating, joining and starting games.
     * It also receives actions from players and passes them on to the correct game.
     */
    class LobbyManager
    {
    public:
        LobbyManager(MessageInterface message_interface) : message_interface(message_interface) {}
        void create_lobby(shared::CreateLobbyRequestMessage request);
        void join_lobby(shared::JoinLobbyRequestMessage request);
        void start_game(shared::StartGameRequestMessage request);
        void receive_action(std::unique_ptr<shared::ActionDecisionMessage> action, MessageInterface &message_interface);
        shared::ReducedGameState get_game_state(std::string game_id, shared::PlayerBase::id_t player);
        const std::map<std::string, Lobby> *get_games() { return &games; };

    private:
        std::map<std::string, Lobby> games;
        MessageInterface message_interface;
    };
} // namespace server
