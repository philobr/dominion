
#include <memory>

#include <rapidjson/document.h>
#include <rapidjson/stringbuffer.h>
#include <rapidjson/writer.h>

#include <shared/message_types.h>
#include <shared/utils/assert.h>
#include <shared/utils/json.h>

using namespace rapidjson;

Document documentFromMsg(const std::string &type, const shared::Message &msg)
{
    Document doc;
    doc.SetObject();

    ADD_STRING_MEMBER(type.c_str(), type);
    ADD_STRING_MEMBER(msg.game_id.c_str(), game_id);
    ADD_STRING_MEMBER(msg.message_id.c_str(), message_id);

    return doc;
}

Document documentFromServerToClientMsg(const std::string &type, const shared::ServerToClientMessage &msg)
{
    return documentFromMsg(type, msg);
}

Document documentFromClientToServerMsg(const std::string &type, const shared::ClientToServerMessage &msg)
{
    Document doc = documentFromMsg(type, msg);
    ADD_STRING_MEMBER(msg.player_id.c_str(), player_id);
    return doc;
}


namespace shared
{

    // ======= SERVER TO CLIENT MESSAGES ======= //

    std::string GameStateMessage::toJson()
    {
        Document doc = documentFromServerToClientMsg("game_state", *this);

        Document game_state_doc = this->game_state->toJson();
        Value game_state_value;
        game_state_value.CopyFrom(game_state_doc, doc.GetAllocator());
        doc.AddMember("game_state", game_state_value, doc.GetAllocator());

        ADD_OPTIONAL_STRING_MEMBER(this->in_response_to, in_response_to);
        return documentToString(doc);
    }

    std::string CreateLobbyResponseMessage::toJson()
    {
        Document doc = documentFromServerToClientMsg("initiate_game_response", *this);
        ADD_OPTIONAL_STRING_MEMBER(this->in_response_to, in_response_to);
        ADD_ARRAY_OF_STRINGS_MEMBER(this->available_cards, available_cards);
        return documentToString(doc);
    }

    std::string JoinLobbyBroadcastMessage::toJson()
    {
        Document doc = documentFromServerToClientMsg("join_game_broadcast", *this);
        ADD_ARRAY_OF_STRINGS_MEMBER(this->players, players);
        return documentToString(doc);
    }

    std::string StartGameBroadcastMessage::toJson()
    {
        Document doc = documentFromServerToClientMsg("start_game_broadcast", *this);
        return documentToString(doc);
    }

    std::string EndGameBroadcastMessage::toJson()
    {
        Document doc = documentFromServerToClientMsg("end_game_broadcast", *this);
        return documentToString(doc);
    }

    std::string ResultResponseMessage::toJson()
    {
        Document doc = documentFromServerToClientMsg("result_response", *this);
        ADD_OPTIONAL_STRING_MEMBER(this->in_response_to, in_response_to);
        ADD_BOOL_MEMBER(this->success, success);
        ADD_OPTIONAL_STRING_MEMBER(this->additional_information, additional_information);
        return documentToString(doc);
    }

    std::string ActionOrderMessage::toJson()
    {
        Document doc = documentFromServerToClientMsg("action_order", *this);

        Document order_json = this->order->toJson();
        doc.AddMember("order", order_json, doc.GetAllocator());

        Document game_state_json = this->game_state->toJson();
        doc.AddMember("game_state", game_state_json, doc.GetAllocator());

        ADD_OPTIONAL_STRING_MEMBER(this->description, description);
        return documentToString(doc);
    }

    // ======= CLIENT TO SERVER MESSAGES ======= //

    std::string GameStateRequestMessage::toJson()
    {
        Document doc = documentFromClientToServerMsg("game_state_request", *this);
        return documentToString(doc);
    }

    std::string CreateLobbyRequestMessage::toJson()
    {
        Document doc = documentFromClientToServerMsg("initiate_game_request", *this);
        return documentToString(doc);
    }

    std::string JoinLobbyRequestMessage::toJson()
    {
        Document doc = documentFromClientToServerMsg("join_game_request", *this);
        return documentToString(doc);
    }

    std::string StartGameRequestMessage::toJson()
    {
        Document doc = documentFromClientToServerMsg("start_game_request", *this);
        ADD_ARRAY_OF_STRINGS_MEMBER(this->selected_cards, selected_cards);
        return documentToString(doc);
    }

    std::string ActionDecisionMessage::toJson()
    {
        Document doc = documentFromClientToServerMsg("action_decision", *this);

        ADD_OPTIONAL_STRING_MEMBER(this->in_response_to, in_response_to);

        ActionDecision *action_decision = this->decision.get();
        if ( PlayActionCardDecision *play_action_card = dynamic_cast<PlayActionCardDecision *>(action_decision) ) {
            ADD_STRING_MEMBER("play_action_card", action);
            ADD_STRING_MEMBER(play_action_card->card_id.c_str(), card_id);
            ADD_ENUM_MEMBER(play_action_card->from, from);
        } else if ( BuyCardDecision *buy_card = dynamic_cast<BuyCardDecision *>(action_decision) ) {
            ADD_STRING_MEMBER("buy_card", action);
            ADD_STRING_MEMBER(buy_card->card.c_str(), card);
        } else if ( dynamic_cast<EndActionPhaseDecision *>(action_decision) != nullptr ) {
            ADD_STRING_MEMBER("end_action_phase", action);
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

        return documentToString(doc);
    }

} // namespace shared
