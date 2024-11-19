#pragma once

#include <server/game/game.h>

namespace server
{
    enum class GamePhase
    {
        ACTION_PHASE,
        BUY_PHASE,
        PLAYING_ACTION_CARD,
    };

    class GameInterface
    {
        std::unique_ptr<GameState> game_state;
        const std::string game_id;
        GamePhase phase;

    public:
        using ptr_t = std::unique_ptr<GameInterface>;
        using response_t = std::unique_ptr<ActionOrder>;

        GameInterface operator=(const GameInterface &other) = delete;
        GameInterface(const GameInterface &other) = delete;
        GameInterface(GameInterface &&other) = default;
        ~GameInterface() = default;

        static ptr_t make(const std::string &game_id, const std::vector<shared::CardBase::id_t> &play_cards,
                          const std::vector<Player::id_t> &player_ids);

        /**
         * @brief Receives an ActionDecision from the Lobby and handles it accordingly.
         * It will return some sort of ServerToClient message, which the lobby manager can pass on.
         *
         * @param action_decision
         * @param in_response_to
         * @param game_id
         * @param affected_player_id
         * @return response_t
         */
        response_t receive_action(std::unique_ptr<shared::ActionDecision> action_decision,
                                  const std::optional<std::string> &in_response_to,
                                  const Player::id_t &affected_player_id);

    private:
        GameInterface(const std::string &game_id, const std::vector<shared::CardBase::id_t> &play_cards,
                      const std::vector<Player::id_t> &player_ids) :
            game_state(std::make_unique<GameState>(play_cards, player_ids)),
            game_id(game_id)
        {}

        response_t handle_action(std::unique_ptr<shared::ActionDecision> action_decision,
                                 const Player::id_t &affected_player_id);

        response_t handle_response(std::unique_ptr<shared::ActionDecision> action_decision,
                                   const std::string &in_response_to, const Player::id_t &affected_player_id);

#define HANDLER(decision_type) /* can also be used to define the func outside of the class */                          \
    response_t decision_type##_handler(std::unique_ptr<shared::decision_type> decision,                                \
                                       const Player::id_t &affected_player_id)

        HANDLER(PlayActionCardDecision);
        HANDLER(BuyCardDecision);
        HANDLER(EndTurnDecision);
        HANDLER(ChooseNCardsFromHandDecision);

#define RESPONSE_HANDLER(decision_type) /* can also be used to define the func outside of the class */                 \
    response_t decision_type##_response_handler(std::unique_ptr<shared::decision_type> decision,                       \
                                                const std::string &in_response_to,                                     \
                                                const Player::id_t &affected_player_id)

        RESPONSE_HANDLER(PlayActionCardDecision);
        RESPONSE_HANDLER(BuyCardDecision);
        RESPONSE_HANDLER(EndTurnDecision);
        RESPONSE_HANDLER(ChooseNCardsFromHandDecision);

    }; // namespace server
} // namespace server
