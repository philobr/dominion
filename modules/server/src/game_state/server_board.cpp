#include <server/game/game_state/server_board.h>
#include <shared/utils/assert.h>

namespace server
{
    ServerBoard::ServerBoard(const std::vector<shared::CardBase::id_t> &kingdom_cards, size_t player_count)
    {
        _ASSERT_EQ(kingdom_cards.size(), size_t(10), "A Game needs to have exactly 10 kingdom cards!");

        initialise_treasure_cards(player_count);
        initialise_victory_cards(player_count);

        for ( auto card_id : kingdom_cards ) {
            this->kingdom_cards.emplace_back(card_id, 10);
        }
    }

    bool ServerBoard::buy(const shared::CardBase::id_t &card_id)
    {
        auto buy_card = [&](const auto &card_id, auto &pile_vector) -> bool {
            return std::any_of(pile_vector.begin(), pile_vector.end(),
                               [card_id](auto &pile)
                               {
                                   if ( pile.card != card_id || pile.count == 0 ) {
                                       return false;
                                   }

                                    --pile.count;
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