#pragma once

#include <map>
#include <memory>
#include <string>

#include "shared/game_state.h"
#include "shared/message_types.h"

#include "game_state.h"
#include "message_interface.h"

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
        void join(shared::PlayerBase::id_t player);
        void start_game();
        void receive_action(std::unique_ptr<shared::ActionDecisionMessage> action, MessageInterface &message_interface);
        shared::ReducedGameState get_game_state(shared::PlayerBase::id_t player);

    private:
        GameState game_state;
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
        void create_lobby(std::string game_id, shared::PlayerBase::id_t game_master);
        void join_lobby(std::string game_id, shared::PlayerBase::id_t player);
        void start_game(std::string game_id);
        void receive_action(std::unique_ptr<shared::ActionDecisionMessage> action, MessageInterface &message_interface);
        shared::ReducedGameState get_game_state(std::string game_id, shared::PlayerBase::id_t player);

    private:
        std::map<std::string, Lobby> games;
        MessageInterface message_interface;
    };
} // namespace server
