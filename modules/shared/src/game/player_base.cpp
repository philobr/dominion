#include <shared/game/game_state/player_base.h>

namespace shared
{
    PlayerBase::PlayerBase(id_t player_id) :
        player_id(player_id), victory_points(0), actions(1), buys(1), treasure(0), draw_pile_size(0)
    {}

    PlayerBase::PlayerBase(const PlayerBase &other) = default;

    PlayerBase &PlayerBase::operator=(const PlayerBase &other)
    {
        if ( this != &other ) {
            player_id = other.player_id;
            victory_points = other.victory_points;
            actions = other.actions;
            buys = other.buys;
            treasure = other.treasure;
            current_card = other.current_card;
            discard_pile = other.discard_pile;
            draw_pile_size = other.draw_pile_size;
        }
        return *this;
    }

    PlayerBase::PlayerBase(PlayerBase &&other) noexcept :
        player_id(std::move(other.player_id)), victory_points(other.victory_points), actions(other.actions),
        buys(other.buys), treasure(other.treasure), current_card(std::move(other.current_card)),
        discard_pile(std::move(other.discard_pile)), draw_pile_size(other.draw_pile_size)
    {}

    PlayerBase &PlayerBase::operator=(PlayerBase &&other) noexcept
    {
        if ( this != &other ) {
            victory_points = other.victory_points;
            actions = other.actions;
            buys = other.buys;
            treasure = other.treasure;
            current_card = std::move(other.current_card);
            discard_pile = std::move(other.discard_pile);
            draw_pile_size = other.draw_pile_size;
        }
        return *this;
    }

    void PlayerBase::decActions()
    {
        if ( actions == 0 ) {
            return;
        }

        actions--;
    }

    void PlayerBase::decBuys()
    {
        if ( buys == 0 ) {
            return;
        }

        buys--;
    }

    void PlayerBase::decTreasure(const unsigned int dec_amount)
    {
        if ( treasure < dec_amount ) {
            return;
        }

        treasure -= dec_amount;
    }

    bool PlayerBase::operator==(const PlayerBase &other) const
    {
        // this is so cursed, sry
        return (player_id == other.player_id) && (victory_points == other.victory_points) &&
                (actions == other.actions) && (buys == other.buys) && (treasure == other.treasure) &&
                (current_card == other.current_card) &&
                ((discard_pile.first == other.discard_pile.first) &&
                 (discard_pile.second == other.discard_pile.second)) &&
                (draw_pile_size == other.draw_pile_size) &&
                ((played_cards.size() == other.played_cards.size()) &&
                 (std::equal(played_cards.begin(), played_cards.end(), other.played_cards.begin())));
    }

} // namespace shared
