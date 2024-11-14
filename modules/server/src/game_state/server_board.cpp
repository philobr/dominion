#include <server/game/game_state/server_board.h>
#include <shared/utils/assert.h>

namespace server
{
    ServerBoard::ServerBoard(const std::vector<shared::CardBase::id_t> &kingdom_cards, size_t player_count) :
        shared::Board(kingdom_cards, player_count)
    {}

    bool ServerBoard::buy(const shared::CardBase::id_t &card_id)
    {
        // helper to search the card in each pile
        auto buy_card = [&](const auto &card_id, auto &pile_vector) -> bool
        {
            return std::any_of(pile_vector.begin(), pile_vector.end(),
                               [card_id](auto &card_pile)
                               {
                                   if ( card_pile.card != card_id || card_pile.count == 0 ) {
                                       return false;
                                   }

                                   --card_pile.count;
                                   return true;
                               });
        };

        return buy_card(card_id, treasure_cards) || buy_card(card_id, victory_cards) ||
                buy_card(card_id, kingdom_cards);
    }

    void ServerBoard::trash_card(const shared::CardBase::id_t &card)
    {
        this->trash.push_back(card);
        return;
    }
} // namespace server