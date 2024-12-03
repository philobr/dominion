#include <server/game/server_board.h>
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

    void ServerBoard::tryBuy(const shared::CardBase::id_t &card_id)
    {
        if ( !has(card_id) ) {
            LOG(WARN) << "tried to buy card: " << card_id << " but its not available";
            throw exception::CardNotAvailable();
        }

        buy(card_id);
    }

    bool ServerBoard::has(const shared::CardBase::id_t &card_id)
    {
        auto has_card = [card_id](const auto &pile)
        {
            if ( pile.find(card_id) != pile.end() ) {
                return pile.find(card_id)->count > 0;
            }
            return false;
        };

        return (curse_card_pile.card_id == card_id && curse_card_pile.count > 0) || has_card(treasure_cards) ||
                has_card(victory_cards) || has_card(kingdom_cards);
    }

    void ServerBoard::buy(const shared::CardBase::id_t &card_id)
    {
        // helper to search the card in each pile
        auto buy_card = [card_id](auto &card_piles) -> bool
        {
            if ( card_piles.find(card_id) != card_piles.end() ) {
                --card_piles.find(card_id)->count;
            }
        };

        buy_card(treasure_cards);
        buy_card(victory_cards);
        buy_card(kingdom_cards);
    }

    void ServerBoard::trashCard(const shared::CardBase::id_t &card) { this->trash.push_back(card); }
} // namespace server