#include <shared/game/game_state/player_base.h>

namespace shared
{
    PlayerBase::PlayerBase(id_t player_id) :
        player_id(player_id), victory_points(0), available_actions(1), available_buys(1), available_treasure(0),
        draw_pile_size(0)
    {}

    PlayerBase::PlayerBase(const PlayerBase &other) = default;

    PlayerBase &PlayerBase::operator=(const PlayerBase &other)
    {
        if ( this != &other ) {
            player_id = other.player_id;
            victory_points = other.victory_points;
            available_actions = other.available_actions;
            available_buys = other.available_buys;
            available_treasure = other.available_treasure;
            current_card = other.current_card;
            discard_pile = other.discard_pile;
            draw_pile_size = other.draw_pile_size;
        }
        return *this;
    }

    PlayerBase::PlayerBase(PlayerBase &&other) noexcept :
        player_id(std::move(other.player_id)), victory_points(other.victory_points),
        available_actions(other.available_actions), available_buys(other.available_buys),
        available_treasure(other.available_treasure), current_card(std::move(other.current_card)),
        discard_pile(std::move(other.discard_pile)), draw_pile_size(other.draw_pile_size)
    {}

    PlayerBase &PlayerBase::operator=(PlayerBase &&other) noexcept
    {
        if ( this != &other ) {
            victory_points = other.victory_points;
            available_actions = other.available_actions;
            available_buys = other.available_buys;
            available_treasure = other.available_treasure;
            current_card = std::move(other.current_card);
            discard_pile = std::move(other.discard_pile);
            draw_pile_size = other.draw_pile_size;
        }
        return *this;
    }

    void PlayerBase::decActions()
    {
        if ( available_actions == 0 ) {
            return;
        }
        available_actions--;
    }

    void PlayerBase::decBuys()
    {
        if ( available_buys == 0 ) {
            return;
        }
        available_buys--;
    }

    void PlayerBase::decTreasure(const unsigned int dec_amount)
    {
        if ( available_treasure < dec_amount ) {
            return;
        }
        available_treasure -= dec_amount;
    }

} // namespace shared
