
#include <memory>

#include <rapidjson/document.h>
#include <rapidjson/stringbuffer.h>
#include <rapidjson/writer.h>

#include <shared/message_types.h>
#include <shared/utils/assert.h>
#include <shared/utils/json.h>

using namespace rapidjson;

Document document_from_msg(const std::string &type, const shared::Message &msg)
{
    Document doc;
    doc.SetObject();

    ADD_STRING_MEMBER(type.c_str(), type);
    ADD_STRING_MEMBER(msg.game_id.c_str(), game_id);
    ADD_STRING_MEMBER(msg.message_id.c_str(), message_id);

    return doc;
}

Document document_from_server_to_client_msg(const std::string &type, const shared::ServerToClientMessage &msg)
{
    return document_from_msg(type, msg);
}

Document document_from_client_to_server_msg(const std::string &type, const shared::ClientToServerMessage &msg)
{
    Document doc = document_from_msg(type, msg);
    ADD_STRING_MEMBER(msg.player_id.c_str(), player_id);
    return doc;
}


namespace shared
{

    // ======= SERVER TO CLIENT MESSAGES ======= //

    std::string GameStateMessage::to_json()
    {
        Document doc = document_from_server_to_client_msg("game_state", *this);
        ADD_OPTIONAL_STRING_MEMBER(this->in_response_to, in_response_to);
        // TODO: Add game state to the document
        return document_to_string(doc);
    }

    std::string CreateLobbyResponseMessage::to_json()
    {
        Document doc = document_from_server_to_client_msg("initiate_game_response", *this);
        ADD_OPTIONAL_STRING_MEMBER(this->in_response_to, in_response_to);
        ADD_ARRAY_OF_STRINGS_MEMBER(this->available_cards, available_cards);
        return document_to_string(doc);
    }

    std::string JoinLobbyBroadcastMessage::to_json()
    {
        Document doc = document_from_server_to_client_msg("join_game_broadcast", *this);
        ADD_STRING_MEMBER(this->player_id.c_str(), player_id);
        return document_to_string(doc);
    }

    std::string StartGameBroadcastMessage::to_json()
    {
        Document doc = document_from_server_to_client_msg("start_game_broadcast", *this);
        return document_to_string(doc);
    }

    std::string EndGameBroadcastMessage::to_json()
    {
        Document doc = document_from_server_to_client_msg("end_game_broadcast", *this);
        return document_to_string(doc);
    }

    std::string ResultResponseMessage::to_json()
    {
        Document doc = document_from_server_to_client_msg("result_response", *this);
        ADD_OPTIONAL_STRING_MEMBER(this->in_response_to, in_response_to);
        ADD_BOOL_MEMBER(this->success, success);
        ADD_OPTIONAL_STRING_MEMBER(this->additional_information, additional_information);
        return document_to_string(doc);
    }

    std::string ActionOrderMessage::to_json()
    {
        Document doc = document_from_server_to_client_msg("action_order", *this);
        ADD_OPTIONAL_STRING_MEMBER(this->description, description);
        return document_to_string(doc);
    }

    // ======= CLIENT TO SERVER MESSAGES ======= //

    std::string GameStateRequestMessage::to_json()
    {
        Document doc = document_from_client_to_server_msg("game_state_request", *this);
        return document_to_string(doc);
    }

    std::string CreateLobbyRequestMessage::to_json()
    {
        Document doc = document_from_client_to_server_msg("initiate_game_request", *this);
        return document_to_string(doc);
    }

    std::string JoinLobbyRequestMessage::to_json()
    {
        Document doc = document_from_client_to_server_msg("join_game_request", *this);
        return document_to_string(doc);
    }

    std::string StartGameRequestMessage::to_json()
    {
        Document doc = document_from_client_to_server_msg("start_game_request", *this);
        ADD_ARRAY_OF_STRINGS_MEMBER(this->selected_cards, selected_cards);
        return document_to_string(doc);
    }

    std::string ActionDecisionMessage::to_json()
    {
        Document doc = document_from_client_to_server_msg("action_decision", *this);

        ADD_OPTIONAL_STRING_MEMBER(this->in_response_to, in_response_to);

        ActionDecision *action_decision = this->decision.get();
        if ( PlayActionCardDecision *play_action_card = dynamic_cast<PlayActionCardDecision *>(action_decision) ) {
            ADD_STRING_MEMBER("play_action_card", action);
            ADD_UINT_MEMBER(play_action_card->cardIndex, card_index);
        } else if ( BuyCardDecision *buy_card = dynamic_cast<BuyCardDecision *>(action_decision) ) {
            ADD_STRING_MEMBER("buy_card", action);
            ADD_STRING_MEMBER(buy_card->card.c_str(), card);
        } else if ( dynamic_cast<EndTurnDecision *>(action_decision) != nullptr ) {
            ADD_STRING_MEMBER("end_turn", action);
        } else if ( ChooseNCardsFromHandDecision *choose_n_cards =
                            dynamic_cast<ChooseNCardsFromHandDecision *>(action_decision) ) {
            ADD_STRING_MEMBER("choose_n_cards_from_hand", action);
            ADD_ARRAY_OF_UINTS_MEMBER(choose_n_cards->cards, cards);
        } else {
            // This code should be unreachable
            _ASSERT_TRUE(false, "Unknown decision type");
        }

        return document_to_string(doc);
    }

} // namespace shared
