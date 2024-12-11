#include <server/game/behaviour_base.h>
#include <shared/utils/utils.h>

namespace server
{
    namespace behaviour
    {
        // TODO: define a stateful behaviour, ask @aaron if you need this

// False positive of clang-tidy
// NOLINTBEGIN(bugprone-macro-parentheses)
#define DEFINE_BEHAVIOUR(name)                                                                                         \
    class name : public server::base::Behaviour                                                                        \
    {                                                                                                                  \
    public:                                                                                                            \
        inline ret_t apply(server::GameState &state,                                                                   \
                           server::base::Behaviour::action_decision_t action_decision = std::nullopt);                 \
    };                                                                                                                 \
    inline server::base::Behaviour::ret_t name::apply(server::GameState &game_state,                                   \
                                                      server::base::Behaviour::action_decision_t action_decision)
// NOLINTEND(bugprone-macro-parentheses)

// False positive of clang-tidy
// NOLINTBEGIN(bugprone-macro-parentheses)
#define DEFINE_TEMPLATED_BEHAVIOUR(name, template_type, template_name)                                                 \
    template <template_type template_name>                                                                             \
    class name : public server::base::Behaviour                                                                        \
    {                                                                                                                  \
    public:                                                                                                            \
        inline ret_t apply(server::GameState &state,                                                                   \
                           server::base::Behaviour::action_decision_t action_decision = std::nullopt);                 \
    };                                                                                                                 \
    template <template_type template_name>                                                                             \
    inline server::base::Behaviour::ret_t name<template_name>::apply(                                                  \
            server::GameState &game_state, server::base::Behaviour::action_decision_t action_decision)
// NOLINTEND(bugprone-macro-parentheses)

// ================================
// HELPER MACROS
// ================================

// call this at the top of your behaviour to log the call.
#define LOG_CALL LOG(INFO) << "Applying " << CLASS_NAME

// call this if the linter is beeing a lil bitch.
#define SUPPRESS_UNUSED_VAR_WARNING(variable) (void)(variable)

#define ASSERT_DECISION                                                                                                \
    if ( action_decision == std::nullopt ) {                                                                           \
        LOG(ERROR) << "Expected a decision, but didnt receive one";                                                    \
        throw std::runtime_error("Expected a decision, but didnt receive one");                                        \
    }

#define ASSERT_NO_DECISION                                                                                             \
    if ( action_decision != std::nullopt ) {                                                                           \
        LOG(ERROR) << "Received a decision, but didnt excpect one";                                                    \
        throw std::runtime_error("Received a decision, but didnt excpect one");                                        \
    }

/**
 * @brief can be used like:
 * auto casted_decision = CAST_DECISION(expected_type);
 * @warning THIS WILL THROW
 */
#define TRY_CAST_DECISION(decision_type)                                                                               \
    [](server::base::Behaviour::action_decision_t &action_decision) -> decision_type *                                 \
    {                                                                                                                  \
        ASSERT_DECISION                                                                                                \
        auto *casted_decision = dynamic_cast<(decision_type) *>(action_decision->get());                               \
        if ( !casted_decision ) {                                                                                      \
            LOG(ERROR) << "Decision has wrong type! Expected: " << utils::demangle(typeid(decision_type).name())       \
                       << ", but got: " << utils::demangle(typeid(*action_decision->get()).name());                    \
            throw std::runtime_error("Decision has wrong type");                                                       \
        }                                                                                                              \
        return casted_decision;                                                                                        \
    }(action_decision)

#define BEHAVIOUR_DONE                                                                                                 \
    finished_behaviour = true;                                                                                         \
    return OrderResponse()


        // ================================
        // BEHAVIOUR IMPLEMENTATIONS
        // ================================
        DEFINE_TEMPLATED_BEHAVIOUR(GainCoins, int, coins)
        {
            LOG_CALL;
            ASSERT_NO_DECISION;

            auto &affected_player = game_state.getCurrentPlayer();
            affected_player.addTreasure(coins);

            BEHAVIOUR_DONE;
        }

        DEFINE_TEMPLATED_BEHAVIOUR(GainBuys, int, buys)
        {
            LOG_CALL;
            ASSERT_NO_DECISION;

            auto &affected_player = game_state.getCurrentPlayer();
            affected_player.addBuys(buys);

            BEHAVIOUR_DONE;
        }

        DEFINE_TEMPLATED_BEHAVIOUR(GainActions, int, actions)
        {
            LOG_CALL;
            ASSERT_NO_DECISION;

            auto &affected_player = game_state.getCurrentPlayer();
            affected_player.addActions(actions);

            BEHAVIOUR_DONE;
        }

        DEFINE_TEMPLATED_BEHAVIOUR(DrawCards, int, n_cards)
        {
            LOG_CALL;
            ASSERT_NO_DECISION;

            auto &affected_player = game_state.getCurrentPlayer();
            affected_player.draw(n_cards);

            BEHAVIOUR_DONE;
        }

        DEFINE_TEMPLATED_BEHAVIOUR(DrawCardsEnemies, int, n_cards)
        {
            LOG_CALL;
            ASSERT_NO_DECISION;

            auto all_player_ids = game_state.getAllPlayerIDs();
            for (auto &player_id : all_player_ids) {
                if (player_id == game_state.getCurrentPlayerId()) {
                    continue;
                }
                auto &affected_player = game_state.getPlayer(player_id);
                affected_player.draw(n_cards);
            }

            BEHAVIOUR_DONE;
        }

#define TODO_IMPLEMENT_ME                                                                                              \
    SUPPRESS_UNUSED_VAR_WARNING(game_state);                                                                           \
    SUPPRESS_UNUSED_VAR_WARNING(action_decision);                                                                      \
    LOG(ERROR) << "BEHAVIOUR " << CLASS_NAME << " IS NOT IMPLEMENTED YET";                                             \
    throw std::runtime_error("not implemented");                                                                       \
    return OrderResponse()

        // ================================
        // PLACEHOLDER BEHAVIOUR
        // ================================
        DEFINE_BEHAVIOUR(NOT_IMPLEMENTED_YET) { TODO_IMPLEMENT_ME; }

        // ================================
        // Behaviours
        // ================================

        DEFINE_BEHAVIOUR(CurseEnemy)
        {
            /**
             * curses are applied clockwise, starting from the player to the right of the cur player
             */

            LOG_CALL;
            ASSERT_NO_DECISION;

            auto all_player_ids = game_state.getAllPlayerIDs();

            // func to give a player a curse
            auto apply_curse = [&](size_t player_index)
            {
                const std::string curse_card = "Curse";
                const auto &player_id = all_player_ids[player_index];

                if ( game_state.getPlayer(player_id).canBlock() ) {
                    return;
                }

                game_state.getBoard()->tryTake(curse_card);
                game_state.getPlayer(player_id).gain(curse_card);
            };

            auto current_player_id = game_state.getCurrentPlayerId();
            const size_t player_count = game_state.getAllPlayerIDs().size();

            // find the idx
            auto current_player_it = std::find(all_player_ids.begin(), all_player_ids.end(), current_player_id);
            size_t current_player_index = std::distance(all_player_ids.begin(), current_player_it);

            // starting at 1 to skip current player_id
            for ( size_t i = 1; i < player_count; ++i ) {
                try {
                    apply_curse((i + current_player_index) % player_count);
                } catch ( const std::exception &e ) {
                    LOG(DEBUG) << "Board ran out of curses, aborting: " << FUNC_NAME;
                    BEHAVIOUR_DONE;
                }
            }

            BEHAVIOUR_DONE;
        }

        DEFINE_TEMPLATED_BEHAVIOUR(GainCardMaxCost, int, max_cost)
        {
            LOG_CALL;

            const bool has_action_decision = action_decision.has_value();
            const auto cur_player_id = game_state.getCurrentPlayerId();

            if ( !has_action_decision ) {
                // send out gain card oder
                return {cur_player_id, std::make_unique<shared::GainFromBoardOrder>(max_cost)};
            }

            auto *gain_decision = dynamic_cast<shared::GainFromBoardDecision *>(action_decision.value().get());
            if ( gain_decision == nullptr ) {
                LOG(ERROR) << FUNC_NAME << " got a wrong decision type! Expected: shared::GainFromBoardDecision";
                throw std::runtime_error("Decision type is not allowed!");
            }

            const auto chosen_card_id = gain_decision->chosen_card;
            game_state.tryGain(cur_player_id, chosen_card_id);

            BEHAVIOUR_DONE;
        }

        DEFINE_BEHAVIOUR(CardToDrawPile)
        {
            LOG_CALL;

            const bool has_action_decision = action_decision.has_value();
            const auto cur_player_id = game_state.getCurrentPlayerId();

            if ( !has_action_decision ) {
                // send out gain card oder
                return {cur_player_id,
                        std::make_unique<shared::ChooseFromHandOrder>(
                                1, 1, shared::ChooseFromOrder::AllowedChoice::DRAW_PILE)};
            }

            auto *deck_choice = dynamic_cast<shared::DeckChoiceDecision *>(action_decision.value().get());
            if ( deck_choice == nullptr ) {
                LOG(ERROR) << FUNC_NAME << " got a wrong decision type! Expected: shared::GainFromBoardDecision";
                throw std::runtime_error("Decision type is not allowed!");
            }

            if ( deck_choice->cards.size() != deck_choice->choices.size() ) {
                LOG(ERROR) << FUNC_NAME << " Size mismatch: each card needs a choice!";
                throw std::runtime_error("Message is ill formed");
            }

            if ( deck_choice->cards.size() != 1 ) {
                LOG(ERROR) << FUNC_NAME << "Expects exactly one choice!";
                throw std::runtime_error("Only one choice is allowed!");
            }

            auto &player = game_state.getCurrentPlayer();
            auto move_card_id = deck_choice->cards.at(0);
            if ( player.hasCard<shared::CardAccess::HAND>(move_card_id) ) {
                LOG(ERROR) << FUNC_NAME << "Player: " << player.getId() << " does not have card: " << move_card_id
                           << " in hand!";
                throw std::runtime_error("Card not found!");
            }

            player.move<shared::CardAccess::HAND, shared::CardAccess::DRAW_PILE_TOP>(move_card_id);
            BEHAVIOUR_DONE;
        }

// ================================
// UNDEF MACROS
// ================================
#undef DEFINE_BEHAVIOUR
#undef BEHAVIOUR_DONE
#undef DEFINE_TEMPLATED_BEHAVIOUR
#undef TRY_CAST_DECISION
#undef LOG_CALL
#undef SUPPRESS_UNUSED_VAR_WARNING
#undef ASSERT_DECISION
#undef ASSERT_NO_DECISION
    } // namespace behaviour
} // namespace server
