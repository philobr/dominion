#pragma once

#include <optional>
#include <string>
#include <vector>

#include "game_state.h"

namespace shared
{

    class Message
    {
    public:
        virtual std::string to_json() = 0;

        std::string game_id;
        std::string message_id;
    };

    /* ======= client -> server ======= */

    class ClientToServerMessage : public Message
    {
    public:
        static std::unique_ptr<ClientToServerMessage> from_json(const std::string &json);

        PlayerBase::id_t player_id;
    };

    class GameStateRequestMessage : public ClientToServerMessage
    {
    public:
        std::string to_json() override;
    };

    class CreateLobbyRequestMessage : public ClientToServerMessage
    {
    public:
        std::string to_json() override;

        PlayerBase::id_t player_id;
    };

    class JoinLobbyRequestMessage : public ClientToServerMessage
    {
    public:
        std::string to_json() override;

        PlayerBase::id_t player_id;
    };

    class StartGameRequestMessage : public ClientToServerMessage
    {
    public:
        std::string to_json() override;

        std::vector<CardBase::id_t> selected_cards;
    };

    class ActionDecisionMessage : public ClientToServerMessage
    {
    public:
        std::string to_json() override;

        // TODO add action decision
    };

    /* ======= server -> client ======= */

    class ServerToClientMessage : public Message
    {
    public:
        /**
         * Parse a JSON string representing the message.
         *
         * Returns nullptr if the JSON is invalid.
         */
        static std::unique_ptr<ServerToClientMessage> from_json(const std::string &json);
    };

    class GameStateMessage : public ServerToClientMessage
    {
    public:
        std::string to_json() override;

        std::optional<std::string> in_response_to;
        ReducedGameState game_state;
    };

    class CreateLobbyResponseMessage : public ServerToClientMessage
    {
    public:
        std::string to_json() override;

        std::optional<std::string> in_response_to;
        std::vector<CardBase::id_t> available_cards;
    };

    class JoinLobbyBroadcastMessage : public ServerToClientMessage
    {
    public:
        std::string to_json() override;

        PlayerBase::id_t player_id;
    };

    class StartGameBroadcastMessage : public ServerToClientMessage
    {
    public:
        std::string to_json() override;
    };

    class EndGameBroadcastMessage : public ServerToClientMessage
    {
    public:
        std::string to_json() override;
        // TODO add player_scores
    };

    class ResultResponseMessage : public ServerToClientMessage
    {
    public:
        std::string to_json() override;

        std::optional<std::string> in_response_to;
        bool success;
        std::optional<std::string> additional_information;
    };

    class ActionOrderMessage : public ServerToClientMessage
    {
    public:
        std::string to_json() override;

        std::optional<std::string> description;
        // TODO add phase and params
    };
} // namespace shared
