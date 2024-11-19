#include <shared/utils/test_helpers.h>

std::vector<shared::CardBase::id_t> get_valid_kingdom_cards()
{
    return {"Village", "Smithy", "Market", "Festival",    "Laboratory",
            "Moat",    "Chapel", "Feast",  "Moneylender", "Remodel"};
}

void set_n_piles_to_empty(shared::Board::pile_container_t &pile_container, size_t n)
{
    size_t i = 0;
    for ( const auto &pile : pile_container ) {
        if ( i >= n ) {
            break;
        }
        pile.count = 0;
        ++i;
    }
}
