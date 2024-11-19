#include <algorithm>
#include <random>
#include <ranges>

#include <server/game/game_state/server_player.h>
#include <shared/utils/assert.h>

namespace server
{
    void Player::shuffle_deck()
    {
        static std::random_device rd; // we only need one instance
        static std::mt19937 gen(rd()); // we only need one instance

        std::shuffle(discard_pile.begin(), discard_pile.end(), gen);

        draw_pile.insert(draw_pile.end(), std::make_move_iterator(discard_pile.begin()),
                         std::make_move_iterator(discard_pile.end()));

        discard_pile.clear();
    }

    shared::ReducedPlayer::ptr_t Player::get_reduced_player()
    {
        return shared::ReducedPlayer::make(static_cast<shared::PlayerBase>(*this), hand_cards);
    }

    shared::ReducedEnemy::ptr_t Player::get_reduced_enemy()
    {
        return shared::ReducedEnemy::make(static_cast<shared::PlayerBase>(*this), hand_cards.size());
    }

    void Player::reset_values()
    {
        actions = 1;
        buys = 1;
        treasure = 0;
        victory_points = 0;
    }

    std::vector<Player::card_id> Player::peek_draw_pile(size_t n)
    {
        if ( draw_pile.size() < n ) {
            shuffle_deck();
        }

        n = std::min(n, draw_pile.size());

        std::vector<card_id> peeked_cards;
        peeked_cards.reserve(n);

        peeked_cards.insert(peeked_cards.end(), draw_pile.begin(), draw_pile.begin() + n);

        return peeked_cards;
    }

    void Player::draw(size_t n)
    {
        if ( draw_pile.size() < n ) {
            shuffle_deck();
        }

        n = std::min(n, draw_pile.size());

        hand_cards.insert(hand_cards.end(), std::make_move_iterator(draw_pile.begin()),
                          std::make_move_iterator(draw_pile.begin() + n));

        draw_pile.erase(draw_pile.begin(), draw_pile.begin() + n);
    }

    void Player::end_turn()
    {
        reset_values();
        discard_pile.insert(discard_pile.end(), std::make_move_iterator(hand_cards.begin()),
                            std::make_move_iterator(hand_cards.end()));

        discard_pile.insert(discard_pile.end(), std::make_move_iterator(played_cards.begin()),
                            std::make_move_iterator(played_cards.end()));

        hand_cards.clear();
        played_cards.clear();

        draw(5);
    }

    void Player::trash(std::vector<Player::card_id> &affected_range, size_t trash_idx)
    {
        _ASSERT_LT(trash_idx, affected_range.size(), "index out of range");

        affected_range.erase(affected_range.begin() + trash_idx);
    }

    void Player::discard(std::vector<Player::card_id> &affected_range, size_t discard_idx)
    {
        _ASSERT_LE(discard_idx, affected_range.size(), "index out of range");

        discard_pile.push_back(affected_range[discard_idx]);
        affected_range.erase(affected_range.begin() + discard_idx);
    }

} // namespace server
