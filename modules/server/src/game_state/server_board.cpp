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

    bool ServerBoard::buy(shared::CardBase::id_t card)
    {
        for ( unsigned i = 0; i < this->kingdom_cards.size(); i++ ) {
            if ( this->kingdom_cards[i].card == card && this->kingdom_cards[i].count > 0 ) {
                this->kingdom_cards[i].count--;
                return true;
            }
        }
        return false;
    }

    void ServerBoard::trash_card(shared::CardBase::id_t card)
    {
        this->trash.push_back(card);
        return;
    }
} // namespace server