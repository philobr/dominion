
#include <shared/message_types.h>
#include <shared/utils/assert.h>

namespace shared
{
    bool Message::operator==(const Message &other) const
    {
        return this->game_id == other.game_id && this->message_id == other.message_id;
    }

    // ======= CLIENT -> SERVER ======= //

    bool ClientToServerMessage::operator==(const ClientToServerMessage &other) const
    {
        return Message::operator==(other) && this->player_id == other.player_id;
    }

    bool GameStateRequestMessage::operator==(const GameStateRequestMessage &other) const
    {
        return ClientToServerMessage::operator==(other);
    }

    bool CreateLobbyRequestMessage::operator==(const CreateLobbyRequestMessage &other) const
    {
        return ClientToServerMessage::operator==(other);
    }

    bool JoinLobbyRequestMessage::operator==(const JoinLobbyRequestMessage &other) const
    {
        return ClientToServerMessage::operator==(other);
    }

    StartGameRequestMessage::StartGameRequestMessage(std::string game_id, std::string message_id,
                                                     PlayerBase::id_t player_id,
                                                     std::vector<CardBase::id_t> selected_cards) :
        ClientToServerMessage(game_id, message_id, player_id),
        selected_cards(selected_cards)
    {
        // Due to a bug in the assert macro we need to cast the size to int
        // TODO: Fix the assert macro
        _ASSERT_EQ(selected_cards.size(), 10, "selected_cards must contain exactly 10 cards");
    }

    bool StartGameRequestMessage::operator==(const StartGameRequestMessage &other) const
    {
        // TODO: It is not quite clear if this is the way we should compare `selected_cards`
        //       We might want this to be a set instead of a vector (or at least sort it before comparing)
        return ClientToServerMessage::operator==(other) && this->selected_cards == other.selected_cards;
    }

    bool ActionDecisionMessage::operator==(const ActionDecisionMessage &other) const
    {
        return ClientToServerMessage::operator==(other) && this->in_response_to == other.in_response_to &&
                *this->decision == *other.decision;
    }

    // ======= SERVER -> CLIENT ======= //

    bool ServerToClientMessage::operator==(const ServerToClientMessage &other) const
    {
        return Message::operator==(other);
    }

    bool GameStateMessage::operator==(const GameStateMessage &other) const
    {
        return ServerToClientMessage::operator==(other) && this->in_response_to == other.in_response_to /* TODO:
                reenable && this->game_state == other.game_state */
                ;
    }

    bool CreateLobbyResponseMessage::operator==(const CreateLobbyResponseMessage &other) const
    {
        // TODO: It is not quite clear if this is the way we should compare `available_cards`
        //       We might want this to be a set instead of a vector (or at least sort it before comparing)
        return ServerToClientMessage::operator==(other) && this->in_response_to == other.in_response_to &&
                this->available_cards == other.available_cards;
    }

    bool JoinLobbyBroadcastMessage::operator==(const JoinLobbyBroadcastMessage &other) const
    {
        return ServerToClientMessage::operator==(other) && this->player_id == other.player_id;
    }

    bool StartGameBroadcastMessage::operator==(const StartGameBroadcastMessage &other) const
    {
        return ServerToClientMessage::operator==(other);
    }

    bool EndGameBroadcastMessage::operator==(const EndGameBroadcastMessage &other) const
    {
        return ServerToClientMessage::operator==(other);
    }

    bool ResultResponseMessage::operator==(const ResultResponseMessage &other) const
    {
        return ServerToClientMessage::operator==(other) && this->in_response_to == other.in_response_to &&
                this->success == other.success && this->additional_information == other.additional_information;
    }

    bool ActionOrderMessage::operator==(const ActionOrderMessage &other) const
    {
        return ServerToClientMessage::operator==(other)
            && *this->order == *other.order
            && this->description == other.description;
    }
} // namespace shared
