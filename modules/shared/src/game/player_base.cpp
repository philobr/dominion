#include <shared/game/game_state/player_base.h>

namespace shared
{
    PlayerBase::PlayerBase(PlayerBase::id_t player_id) :
        player_id(player_id), victory_points(0), available_actions(1), available_buys(1), available_treasure(0),
        current_card(""), discard_pile({"", 0}), draw_pile_size(0)
    {}

    PlayerBase::PlayerBase() :
        player_id(""), victory_points(0), available_actions(1), available_buys(1), available_treasure(0),
        current_card(""), discard_pile({"", 0}), draw_pile_size(0)
    {}

    PlayerBase::PlayerBase(const PlayerBase &ohter) :
        player_id(ohter.player_id), victory_points(ohter.victory_points), available_actions(ohter.available_actions),
        available_buys(ohter.available_buys), available_treasure(ohter.available_treasure),
        current_card(ohter.current_card), discard_pile(ohter.discard_pile), draw_pile_size(ohter.draw_pile_size)
    {}

} // namespace shared
