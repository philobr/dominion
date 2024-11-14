#include <server/game/game_state/server_board.h>
#include <shared/utils/assert.h>

namespace server
{
    ServerBoard::ptr_t ServerBoard::make(const std::vector<shared::CardBase::id_t> &kingdom_cards, size_t player_count)
    {
        _ASSERT_TRUE((2 <= player_count && player_count <= 4), "Players must be in [2, 4]");
        return ptr_t(new ServerBoard(kingdom_cards, player_count));
    }

    ServerBoard::ServerBoard(const std::vector<shared::CardBase::id_t> &kingdom_cards, size_t player_count) :
        shared::Board(kingdom_cards, player_count)
    {}

    shared::Board::ptr_t ServerBoard::getReduced()
    {
        return std::static_pointer_cast<shared::Board>(shared_from_this());
    }

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