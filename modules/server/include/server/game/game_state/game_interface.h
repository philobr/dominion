#pragma once
#include <server/game/game.h>
#include <server/network/message_interface.h> // TODO: should be removed later!

#include <unordered_map>

namespace server
{
    class GameInterface
    {
        // 1. can we know IF we will need a response to a message?
        // 2. can we know IF a message chain is completed?
        // IF those things are possible we can store and gargage collect messages for later lookup here
        // (this works with unique_ptr, because const unique_ptr& passes a non owning view)
        // => this way we could store messages (or at least the relevant info) in this class and access them

        using message_id = int; // placeholder
        using message_uniuqe_ptr = int; // placeholder
        std::unordered_map<message_id, message_uniuqe_ptr> message_history;

        std::unique_ptr<GameState> game_state;

        const std::string game_id; // needed for responses!
    public:
        using ptr_t = std::unique_ptr<GameInterface>;
        using response_t = std::unique_ptr<shared::ResultResponseMessage>;

        GameInterface operator=(const GameInterface &other) = delete;
        GameInterface(const GameInterface &other) = delete;
        GameInterface(GameInterface &&other) = default;
        ~GameInterface() = default;

        static ptr_t make(const std::vector<shared::CardBase::id_t> &play_cards,
                          const std::vector<Player::id_t> &player_ids);

        // this is for mvp only, maybe remove it completely
        void receive_action(std::unique_ptr<shared::ActionDecision> action_decision,
                            MessageInterface &message_interface, const Player::id_t &affected_player_id,
                            const std::optional<std::string> &in_response_to);

        // this mostly serves as a placeholder to convey the idea
        struct MessageMetadata
        {
            const Player::id_t &affected_player_id;
            const std::string game_id;
            const std::optional<std::string> in_response_to;
        };

        response_t receive_action(std::unique_ptr<shared::ActionDecision> action_decision,
                                  const MessageMetadata &meatdata);

    private:
        GameInterface(const std::vector<shared::CardBase::id_t> &play_cards,
                      const std::vector<Player::id_t> &player_ids) :
            game_state(std::make_unique<GameState>(play_cards, player_ids))
        {}

        // ================================================================================================================================
        // Those two figure out what kind of decision we got and then forward it
        // --------------------------------------------------------------------------------------------------------------------------------
        response_t handle_action(std::unique_ptr<shared::ActionDecision> action_decision,
                                 const Player::id_t &affected_player_id);

        response_t handle_response(std::unique_ptr<shared::ActionDecision> action_decision,
                                   const Player::id_t &affected_player_id, const std::string &in_response_to);
        // ================================================================================================================================


        // ================================================================================================================================
        // Handle specific decisions here, we could also add the std::optional<std::string> in_response_to here.
        // --------------------------------------------------------------------------------------------------------------------------------
        response_t PlayActionCardDecision_handler(std::unique_ptr<shared::PlayActionCardDecision> decision,
                                                  const Player::id_t &affected_player_id);

        response_t BuyCardDecision_handler(std::unique_ptr<shared::BuyCardDecision> decision,
                                           const Player::id_t &affected_player_id);

        response_t EndTurnDecision_handler(std::unique_ptr<shared::EndTurnDecision> decision,
                                           const Player::id_t &affected_player_id);

        response_t ChooseNCardsFromHandDecision_handler(std::unique_ptr<shared::ChooseNCardsFromHandDecision> decision,
                                                        const Player::id_t &affected_player_id);
        // ================================================================================================================================


        // ================================================================================================================================
        // Handle responses to previous actions here (idk if this differenciation makes sense, but id rather delete it
        // than refactor)
        // --------------------------------------------------------------------------------------------------------------------------------
        response_t PlayActionCardDecision_response_handler(std::unique_ptr<shared::PlayActionCardDecision> decision,
                                                           const Player::id_t &affected_player_id,
                                                           const std::string &in_response_to);

        response_t BuyCardDecision_response_handler(std::unique_ptr<shared::BuyCardDecision> decision,

                                                    const Player::id_t &affected_player_id,
                                                    const std::string &in_response_to);

        response_t EndTurnDecision_response_handler(std::unique_ptr<shared::EndTurnDecision> decision,

                                                    const Player::id_t &affected_player_id,
                                                    const std::string &in_response_to);

        response_t
        ChooseNCardsFromHandDecision_response_handler(std::unique_ptr<shared::ChooseNCardsFromHandDecision> decision,
                                                      const Player::id_t &affected_player_id,
                                                      const std::string &in_response_to);
        // ================================================================================================================================
    }; // namespace server
} // namespace server