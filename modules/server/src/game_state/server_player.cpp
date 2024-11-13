#include <algorithm>
#include <random>
#include <server/game/game_state/server_player.h>
#include <shared/utils/assert.h>

namespace server
{
    void Player::shuffle_deck()
    {
        static std::random_device rd; // only need one instance
        static std::mt19937 gen(rd()); // only need one instance

        std::shuffle(discard_pile.begin(), discard_pile.end(), gen);

        draw_pile.insert(draw_pile.end(), discard_pile.begin(), discard_pile.end());
        discard_pile.clear();
    }

    shared::ReducedPlayer Player::get_reduced_player()
    {
        return shared::ReducedPlayer(static_cast<shared::PlayerBase>(*this), hand_cards);
    }

    shared::ReducedEnemy Player::get_reduced_enemy()
    {
        return shared::ReducedEnemy(static_cast<shared::PlayerBase>(*this), hand_cards.size());
    }

    void Player::reset_values()
    {
        available_actions = 1;
        available_buys = 1;
        available_treasure = 0;
    }

    std::vector<Player::card_id> Player::peek_draw_pile(size_t n)
    {
        if ( draw_pile.size() < n ) {
            shuffle_deck();
        }

        n = std::min(n, draw_pile.size());

        std::vector<card_id> peeked_cards(n);
        for ( size_t i = 0; i < n; ++i ) {
            peeked_cards[i] = draw_pile[i];
        }

        return peeked_cards;
    }

    void Player::end_turn()
    {
        reset_values();
        discard_pile.insert(discard_pile.end(), hand_cards.begin(), hand_cards.end());
        discard_pile.insert(discard_pile.end(), played_cards.begin(), played_cards.end());

        hand_cards.clear();
        played_cards.clear();

        draw(5);
    }

    void Player::draw(size_t n)
    {
        if ( draw_pile.size() < n ) {
            shuffle_deck();
        }

        n = std::min(n, draw_pile.size());

        for ( size_t i = 0; i < n; ++i ) {
            hand_cards.push_back(draw_pile.front());
            draw_pile.pop_front();
        }
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

    void Player::add(std::vector<Player::card_id> &range, const card_id &card) { range.push_back(card); }

} // namespace server
