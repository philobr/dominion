#pragma once

#include <memory>
#include <optional>
#include <string>
#include <vector>

#include <shared/action_decision.h>
#include <shared/action_order.h>
#include <shared/game/game_state/player_base.h>
#include <shared/game/game_state/reduced_game_state.h>
#include <shared/utils/uuid_generator.h>

namespace shared
{

    class Message
    {
    public:
        virtual ~Message() = default;
        virtual std::string toJson() = 0;

        std::string game_id;
        std::string message_id;

    protected:
        Message(std::string game_id, std::string message_id = UuidGenerator::generateUuidV4()) :
            game_id(game_id), message_id(message_id)
        {}
        bool operator==(const Message &other) const;
    };

    /* ======= client -> server ======= */

    class ClientToServerMessage : public Message
    {
    public:
        ~ClientToServerMessage() override = default;
        std::string toJson() override = 0;
        static std::unique_ptr<ClientToServerMessage> fromJson(const std::string &json);

        PlayerBase::id_t player_id;

    protected:
        ClientToServerMessage(std::string game_id, PlayerBase::id_t player_id,
                              std::string message_id = UuidGenerator::generateUuidV4()) :
            Message(game_id, message_id),
            player_id(player_id)
        {}
        bool operator==(const ClientToServerMessage &other) const;
    };

    class GameStateRequestMessage final : public ClientToServerMessage
    {
    public:
        GameStateRequestMessage(std::string game_id, PlayerBase::id_t player_id,
                                std::string message_id = UuidGenerator::generateUuidV4()) :
            ClientToServerMessage(game_id, player_id, message_id)
        {}
        ~GameStateRequestMessage() override = default;
        std::string toJson() override;
        bool operator==(const GameStateRequestMessage &other) const;
    };

    class CreateLobbyRequestMessage final : public ClientToServerMessage
    {
    public:
        CreateLobbyRequestMessage(std::string game_id, PlayerBase::id_t player_id,
                                  std::string message_id = UuidGenerator::generateUuidV4()) :
            ClientToServerMessage(game_id, player_id, message_id)
        {}
        ~CreateLobbyRequestMessage() override = default;
        std::string toJson() override;
        bool operator==(const CreateLobbyRequestMessage &other) const;
    };

    class JoinLobbyRequestMessage final : public ClientToServerMessage
    {
    public:
        ~JoinLobbyRequestMessage() override = default;
        JoinLobbyRequestMessage(std::string game_id, PlayerBase::id_t player_id,
                                std::string message_id = UuidGenerator::generateUuidV4()) :
            ClientToServerMessage(game_id, player_id, message_id)
        {}
        std::string toJson() override;
        bool operator==(const JoinLobbyRequestMessage &other) const;
    };

    class StartGameRequestMessage final : public ClientToServerMessage
    {
    public:
        ~StartGameRequestMessage() override = default;
        /**
         * @param selected_cards The 10 cards selected by the game master to play with.
         * The size of the vector must be 10. Otherwise, an assertion will fail.
         */
        StartGameRequestMessage(std::string game_id, PlayerBase::id_t player_id,
                                std::vector<CardBase::id_t> selected_cards,
                                std::string message_id = UuidGenerator::generateUuidV4());
        std::string toJson() override;
        bool operator==(const StartGameRequestMessage &other) const;

        std::vector<CardBase::id_t> selected_cards;
    };

    class ActionDecisionMessage final : public ClientToServerMessage
    {
    public:
        ~ActionDecisionMessage() override = default;
        ActionDecisionMessage(std::string game_id, PlayerBase::id_t player_id, std::unique_ptr<ActionDecision> decision,
                              std::optional<std::string> in_response_to = std::nullopt,
                              std::string message_id = UuidGenerator::generateUuidV4()) :
            ClientToServerMessage(game_id, player_id, message_id),
            decision(std::move(decision)), in_response_to(in_response_to)
        {}
        std::string toJson() override;
        bool operator==(const ActionDecisionMessage &other) const;

        std::unique_ptr<ActionDecision> decision;
        std::optional<std::string> in_response_to;
    };

    /* ======= server -> client ======= */

    class ServerToClientMessage : public Message
    {
    public:
        ~ServerToClientMessage() override = default;
        /**
         * Parse a JSON string representing the message.
         *
         * Returns nullptr if the JSON is invalid.
         */
        std::string toJson() override = 0;
        static std::unique_ptr<ServerToClientMessage> fromJson(const std::string &json);

    protected:
        ServerToClientMessage(std::string game_id, std::string message_id = UuidGenerator::generateUuidV4()) :
            Message(game_id, message_id)
        {}
        bool operator==(const ServerToClientMessage &other) const;
    };

    class GameStateMessage final : public ServerToClientMessage
    {
    public:
        ~GameStateMessage() override = default;
        GameStateMessage(std::string game_id, std::unique_ptr<reduced::GameState> game_state,
                         std::optional<std::string> in_response_to = std::nullopt,
                         std::string message_id = UuidGenerator::generateUuidV4()) :

            ServerToClientMessage(game_id, message_id),
            game_state(std::move(game_state)), in_response_to(in_response_to)
        {}
        std::string toJson() override;
        bool operator==(const GameStateMessage &other) const;

        std::unique_ptr<reduced::GameState> game_state;
        std::optional<std::string> in_response_to;
    };

    class CreateLobbyResponseMessage final : public ServerToClientMessage
    {
    public:
        ~CreateLobbyResponseMessage() override = default;
        CreateLobbyResponseMessage(std::string game_id, std::vector<CardBase::id_t> available_cards,
                                   std::optional<std::string> in_response_to = std::nullopt,
                                   std::string message_id = UuidGenerator::generateUuidV4()) :
            ServerToClientMessage(game_id, message_id),
            available_cards(available_cards), in_response_to(in_response_to)
        {}
        std::string toJson() override;
        bool operator==(const CreateLobbyResponseMessage &other) const;

        std::vector<CardBase::id_t> available_cards;
        std::optional<std::string> in_response_to;
    };

    class JoinLobbyBroadcastMessage final : public ServerToClientMessage
    {
    public:
        ~JoinLobbyBroadcastMessage() override = default;
        JoinLobbyBroadcastMessage(std::string game_id, std::vector<shared::PlayerBase::id_t> players,
                                  std::string message_id = UuidGenerator::generateUuidV4()) :
            ServerToClientMessage(game_id, message_id),
            players(players)
        {}
        std::string toJson() override;
        bool operator==(const JoinLobbyBroadcastMessage &other) const;
        std::vector<shared::PlayerBase::id_t> players;
    };

    class StartGameBroadcastMessage final : public ServerToClientMessage
    {
    public:
        ~StartGameBroadcastMessage() override = default;
        StartGameBroadcastMessage(std::string game_id, std::string message_id = UuidGenerator::generateUuidV4()) :
            ServerToClientMessage(game_id, message_id)
        {}
        std::string toJson() override;
        bool operator==(const StartGameBroadcastMessage &other) const;
    };

    class EndGameBroadcastMessage final : public ServerToClientMessage
    {
    public:
        ~EndGameBroadcastMessage() override = default;
        EndGameBroadcastMessage(std::string game_id, std::string message_id = UuidGenerator::generateUuidV4()) :
            ServerToClientMessage(game_id, message_id)
        {}
        std::string toJson() override;
        bool operator==(const EndGameBroadcastMessage &other) const;
        // TODO add player_scores
    };

    class ResultResponseMessage final : public ServerToClientMessage
    {
    public:
        ~ResultResponseMessage() override = default;
        ResultResponseMessage(std::string game_id, bool success,
                              std::optional<std::string> in_response_to = std::nullopt,
                              std::optional<std::string> additional_information = std::nullopt,
                              std::string message_id = UuidGenerator::generateUuidV4()) :
            ServerToClientMessage(game_id, message_id),
            success(success), in_response_to(in_response_to), additional_information(additional_information)
        {}
        std::string toJson() override;
        bool operator==(const ResultResponseMessage &other) const;

        bool success;
        std::optional<std::string> in_response_to;
        std::optional<std::string> additional_information;
    };

    class ActionOrderMessage final : public ServerToClientMessage
    {
    public:
        ~ActionOrderMessage() override = default;
        ActionOrderMessage(std::string game_id, std::unique_ptr<ActionOrder> order,
                           std::optional<std::string> description = std::nullopt,
                           std::string message_id = UuidGenerator::generateUuidV4()) :
            ServerToClientMessage(game_id, message_id),
            order(std::move(order)), description(description)
        {}
        std::string toJson() override;
        bool operator==(const ActionOrderMessage &other) const;

        std::unique_ptr<ActionOrder> order;
        std::optional<std::string> description;
    };
} // namespace shared
