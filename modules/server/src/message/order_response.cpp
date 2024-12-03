#include <server/message/order_response.h>
#include <shared/utils/logger.h>

void OrderResponse::addOrder(const shared::PlayerBase::id_t &player_id, std::unique_ptr<shared::ActionOrder> order)
{
    if ( hasOrder(player_id) ) {
        LOG(ERROR) << "Tried to give player " + player_id + " two orders at once";
        throw std::runtime_error("Tried to give player " + player_id + " two orders at once");
    }
    response_map.emplace(player_id, std::move(order));
}