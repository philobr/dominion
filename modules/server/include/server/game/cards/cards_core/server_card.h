#pragma once

#include <memory>
#include <server/game/game_state/game_state.h>
#include <shared/game/cards/card_base.h>
#include <shared/utils/assert.h>

namespace server
{
    /**
     * @brief I needed a new CardBase that provides play(), toString() to be able to play a card.
     * This way we can have a common ptr type which also provides the needed funcitons to play a card.
     */
    class CardBase : public shared::CardBase
    {
    public:
        using id_t = shared::CardBase::id_t;
        using ptr_t = std::unique_ptr<CardBase>;

        CardBase(id_t id, shared::CardType type, unsigned int cost) : shared::CardBase(id, type, cost) {}
        virtual ~CardBase() = default;

        /**
         * @brief This function executes the behaviour at idx 'behaviour_idx'. It applies its logic onto gamestate and
         * affected player.
         *
         * @param gamestate
         * @param affected_player
         * @param idx the idx of the behaviour that is to be executed
         * @return int The idx of the next card, needs to be checked with size()
         */
        virtual int play(server::GameState &gamestate, server::Player::id_t &affected_player,
                         std::size_t behaviour_idx) = 0;
    };

    template <typename... Behaviours>
    class Card : public CardBase
    {
        std::tuple<Behaviours...> behaviours;

    public:
        using id_t = CardBase::id_t;
        using ptr_t = CardBase::ptr_t;

        Card(id_t id, shared::CardType type, unsigned int cost) : CardBase(id, type, cost) {}

        static constexpr size_t size() { return std::tuple_size<decltype(behaviours)>{}; }

        /**
         * @brief This function executes the behaviour at idx 'behaviour_idx'. It applies its logic onto gamestate and
         * affected player.
         *
         * @param gamestate
         * @param affected_player
         * @param idx the idx of the behaviour that is to be executed
         * @return int The idx of the next card, needs to be checked with size()
         */
        int play(server::GameState &gamestate, server::Player::id_t &affected_player,
                 std::size_t behaviour_idx = 0) override;

    private:
        static constexpr bool is_valid(std::size_t idx) { return idx >= 0 && idx < size(); }

        /**
         * @brief Applies a behaviour at a specific index which is passed at runtime.
         *
         * @tparam I variadic index sequence
         * @param gamestate
         * @param affected_player
         * @param behaviour_idx
         * @return true the applied behaviour is finished
         * @return false the applied behaviour is to be called again
         */
        template <std::size_t... I>
        bool applyBehaviourAtIndex(GameState &gamestate, Player::id_t affected_player, std::size_t behaviour_idx,
                                   std::index_sequence<I...>);

        /**
         * @brief This function applies a specific behaviour. Its basically the basecase for applyBehaviourAtIndex
         *
         * @param behaviour
         * @param gamestate
         * @param affected_player
         * @return true the behaviour is finished
         * @return false the behaviour is to be called again
         */
        template <typename Behaviour>
        bool applyBehaviour(Behaviour &behaviour, GameState &gamestate, Player::id_t &affected_player);
    };

    template <typename... Behaviours>
    int Card<Behaviours...>::play(GameState &gamestate, Player::id_t &affected_player, std::size_t behaviour_idx)
    {
        if ( !is_valid(behaviour_idx) ) {
            return size();
        }

        bool finished = applyBehaviourAtIndex(gamestate, affected_player, behaviour_idx,
                                              std::index_sequence_for<Behaviours...>{});

        return behaviour_idx + (finished ? 1 : 0);
    }

    template <typename... Behaviours>
    template <std::size_t... I>
    bool Card<Behaviours...>::applyBehaviourAtIndex(GameState &gamestate, Player::id_t affected_player,
                                                    std::size_t behaviour_idx, std::index_sequence<I...>)
    {
        bool finished = false;
        // this fold expression checks each index in I... to find the matching behaviour.
        // when the index matches behaviour_idx, we execute applyBehaviour for that behaviour.
        ((void)(I == behaviour_idx
                        ? (finished = applyBehaviour(std::get<I>(behaviours), gamestate, affected_player), true)
                        : false),
         ...);

        return finished;
    }

    template <typename... Behaviours>
    template <typename Behaviour>
    bool Card<Behaviours...>::applyBehaviour(Behaviour &behaviour, GameState &gamestate, Player::id_t &affected_player)
    {
        return behaviour.apply(gamestate, affected_player);
    }

} // namespace server
