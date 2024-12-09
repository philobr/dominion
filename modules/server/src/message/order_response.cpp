#include <server/message/order_response.h>
#include <shared/utils/logger.h>

bool OrderResponse::isGameOver() const { return _game_over; }

std::vector<shared::PlayerResult> OrderResponse::getResults() const
{
    if ( !_game_over ) {
        throw std::runtime_error("Tried to get results from OrderResponse, but game is not over.");
    }
    return _player_results;
}

void OrderResponse::setGameOver(std::vector<shared::PlayerResult> results)
{
    _game_over = true;
    _player_results = std::move(results);
}

void OrderResponse::addOrder(const shared::PlayerBase::id_t &player_id, std::unique_ptr<shared::ActionOrder> order)
{
    if ( hasOrder(player_id) ) {
        LOG(ERROR) << "Tried to give player " + player_id + " two orders at once";
        throw std::runtime_error("Tried to give player " + player_id + " two orders at once");
    }
    response_map.emplace(player_id, std::move(order));
}
