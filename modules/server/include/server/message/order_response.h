#pragma once

#include <unordered_map>

#include <shared/action_order.h>
#include <shared/game/game_state/player_base.h>
#include <shared/player_result.h>

/**
 * @brief This is just a wrapper for a map<player_id, action_order>
 *
 * Its the return type for all functions in game_interface and in all behaviours.
 * The lobby can use this to update send out orders to players.
 */
class OrderResponse
{
    bool _game_over = false;

    /**
     * @brief Stores the results of the players if the game is over.
     *
     * If `game_over` is true, this vector will be filled with the results of the players.
     * If `game_over` is false, this vector will be empty.
     */
    std::vector<shared::PlayerResult> _player_results;

    /**
     * @brief Stores the orders for each player.
     *
     * If `game_over` is true, this map will be empty.
     * If `game_over` is false, this map will be filled with the orders for each player.
     */
    std::unordered_map<shared::PlayerBase::id_t, std::unique_ptr<shared::ActionOrder>> response_map;

public:
    /**
     * @brief Constructor to allow to create OrderResponses with as many player/order pairs as one wants.
     * @warning you can't give a player more than one order!
     */
    template <typename... Args>
    OrderResponse(Args &&...args);

    OrderResponse() = default;
    OrderResponse(const OrderResponse &) = default;
    OrderResponse(OrderResponse &&) noexcept = default;
    OrderResponse &operator=(const OrderResponse &) = default;
    OrderResponse &operator=(OrderResponse &&) noexcept = default;
    ~OrderResponse() = default;

    /**
     * @brief Check if the game is over.
     *
     * If this returns true, the game is over and the results can be retrieved with `getResults()`.
     * If this returns false, the game is not over and the orders can be retrieved with `getOrder()`.
     */
    bool isGameOver() const;

    /**
     * @brief Get the results of the players if the game is over.
     *
     * This should only be called if `isGameOver()` returns true.
     */
    std::vector<shared::PlayerResult> getResults() const;

    /**
     * @brief Set the game over flag and store the results of the players.
     */
    void setGameOver(std::vector<shared::PlayerResult> results);

    bool empty() const { return response_map.empty(); }
    bool hasOrder(const shared::PlayerBase::id_t &player_id) const { return response_map.count(player_id) != 0; }
    auto getOrder(const shared::PlayerBase::id_t &player_id) { return std::move(response_map.at(player_id)); }

    template <typename DerivedOrder>
    void addOrder(const shared::PlayerBase::id_t &player_id, std::unique_ptr<DerivedOrder> order);

private:
    void addOrder(const shared::PlayerBase::id_t &player_id, std::unique_ptr<shared::ActionOrder> order);

    /**
     * @brief Unfolds the initializer_list received from the constructor and adds the orders.
     * This fucky stuff only exists because i wanted to be able to do:
     *
     * return OrderResponse("id1", order1, "id2", order2);
     *
     * instead of
     *
     * OrderResponse response;
     * response.add("id1", order1);
     * response.add("id2", order2);
     */
    template <typename T1, typename T2, typename... Rest>
    void addOrders(T1 &&player_id, T2 &&order, Rest &&...rest);
};


template <typename... Args>
inline OrderResponse::OrderResponse(Args &&...args)
{
    static_assert(sizeof...(args) % 2 == 0, "Must provide pairs of player ID and ActionOrder.");
    addOrders(std::forward<Args>(args)...);
}

template <typename DerivedOrder>
inline void OrderResponse::addOrder(const shared::PlayerBase::id_t &player_id, std::unique_ptr<DerivedOrder> order)
{
    static_assert(std::is_base_of_v<shared::ActionOrder, DerivedOrder>,
                  "DerivedOrder must be derived from ActionOrder");
    addOrder(player_id, std::unique_ptr<shared::ActionOrder>(std::move(order)));
}

template <typename T1, typename T2, typename... Rest>
inline void OrderResponse::addOrders(T1 &&player_id, T2 &&order, Rest &&...rest)
{
    static_assert(std::is_same_v<std::decay_t<T1>, shared::PlayerBase::id_t>, "Expected player_id to be a string.");
    static_assert(std::is_base_of_v<shared::ActionOrder, typename std::decay_t<T2>::element_type>,
                  "Expected order to be derived from ActionOrder.");

    addOrder(std::forward<T1>(player_id), std::unique_ptr<shared::ActionOrder>(std::forward<T2>(order)));

    if constexpr ( sizeof...(Rest) > 0 ) {
        addOrders(std::forward<Rest>(rest)...);
    }
}
