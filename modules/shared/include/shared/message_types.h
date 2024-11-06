#pragma once

#include <optional>
#include <string>
#include <vector>

#include "action_decision.h"
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

        std::optional<std::string> in_response_to;
        PlayerBase::id_t player_id;
        std::unique_ptr<ActionDecision> decision;
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
        GameStateMessage() {}
        std::string to_json() override;

        std::optional<std::string> in_response_to;
        ReducedGameState game_state;
    };

    class CreateLobbyResponseMessage : public ServerToClientMessage
    {
    public:
        CreateLobbyResponseMessage(std::vector<CardBase::id_t> available_cards, std::optional<std::string> in_response_to = std::nullopt)
            : in_response_to(in_response_to), available_cards(available_cards)
        {}
        std::string to_json() override;

        std::optional<std::string> in_response_to;
        std::vector<CardBase::id_t> available_cards;
    };

    class JoinLobbyBroadcastMessage : public ServerToClientMessage
    {
    public:

        JoinLobbyBroadcastMessage(PlayerBase::id_t join_id) : player_id(join_id) {}
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
        ResultResponseMessage(bool success, std::optional<std::string> in_response_to = std::nullopt, std::optional<std::string> additional_information = std::nullopt)
            : in_response_to(in_response_to), success(success), additional_information(additional_information)
        {}
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
