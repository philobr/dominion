#include <shared/game/game_state/player_base.h>

namespace shared
{
    ReducedPlayer::ReducedPlayer(const PlayerBase &player, const std::vector<CardBase::id_t> &hand_cards) :
        PlayerBase(player), hand_cards(std::move(hand_cards))
    {}

    ReducedPlayer::ptr_t ReducedPlayer::make(const PlayerBase &player, std::vector<CardBase::id_t> hand_cards)
    {
        return ptr_t(new ReducedPlayer(player, hand_cards));
    }

    const std::vector<CardBase::id_t> &ReducedPlayer::getHandCards() const { return hand_cards; }

    ReducedEnemy::ReducedEnemy(const PlayerBase &player, unsigned int hand) : PlayerBase(player), hand_size(hand) {}

    ReducedEnemy::ptr_t ReducedEnemy::make(const PlayerBase &player, unsigned int hand_size)
    {
        return ptr_t(new ReducedEnemy(player, hand_size));
    }

    unsigned int ReducedEnemy::getHandSize() const { return hand_size; }

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
