#include "server_player.h"

#pragma region UTILS
template <enum shared::CardAccess PILE>
inline std::vector<shared::CardBase::id_t> &server::Player::getMutable()
{
    if constexpr ( PILE == shared::DISCARD_PILE ) {
        return discard_pile;
    } else if constexpr ( PILE == shared::HAND ) {
        return hand_cards;
    } else if constexpr ( PILE == shared::PLAYED_CARDS ) {
        return played_cards;
    } else if constexpr ( PILE == shared::STAGED_CARDS ) {
        return staged_cards;
    } else if constexpr ( PILE == shared::DRAW_PILE_TOP ) {
        return draw_pile;
    } else if constexpr ( PILE == shared::DRAW_PILE_BOTTOM ) {
        return draw_pile;
    } else {
        // should only happen for trash pile
        throw std::invalid_argument("Invalid pile specified or pile is not accessible.");
    }
}

template <enum shared::CardAccess PILE>
inline const std::vector<shared::CardBase::id_t> &server::Player::get() const
{
    if constexpr ( PILE == shared::DISCARD_PILE ) {
        return discard_pile;
    } else if constexpr ( PILE == shared::HAND ) {
        return hand_cards;
    } else if constexpr ( PILE == shared::PLAYED_CARDS ) {
        return played_cards;
    } else if constexpr ( PILE == shared::STAGED_CARDS ) {
        return staged_cards;
    } else if constexpr ( PILE == shared::DRAW_PILE_TOP ) {
        return draw_pile;
    } else if constexpr ( PILE == shared::DRAW_PILE_BOTTOM ) {
        return draw_pile;
    } else {
        // should only happen for trash pile
        LOG(ERROR) << "Tried to access an invalid player pile";
        throw std::invalid_argument("Invalid pile specified or pile is not accessible.");
    }
}

template <enum shared::CardAccess PILE>
inline void server::Player::shuffle()
{
    static std::random_device rd; // we only need one instance
    static std::mt19937 gen(rd()); // we only need one instance

    auto &cards = getMutable<PILE>();
    std::shuffle(cards.begin(), cards.end(), gen);
}

template <enum shared::CardAccess PILE>
inline bool server::Player::hasCard(const shared::CardBase::id_t &card_id) const
{
    const auto &cards = get<PILE>();
    return std::find_if(cards.begin(), cards.end(), [card_id](const auto &id) { return id == card_id; }) != cards.end();
}

template <enum shared::CardAccess PILE>
inline size_t server::Player::getIndex(const shared::CardBase::id_t &card_id) const
{
    if ( !hasCard<PILE>(card_id) ) {
        LOG(WARN) << "Card: " << card_id << " does not exist!";
        throw std::runtime_error("Tried to get the index of a nonexistent card");
    }

    const auto &cards = get<PILE>();
    const auto idx = std::find_if(cards.begin(), cards.end(), [card_id](const auto &id) { return id == card_id; });

    return std::distance(cards.begin(), idx);
}

#pragma region ADD

template <enum shared::CardAccess TO, typename Iterator>
inline void server::Player::add(Iterator begin, Iterator end)
{
    if constexpr ( TO == shared::TRASH ) {
        LOG(ERROR) << "Can't add cards to the trash Pile from the player! (you need to trash cards using the board)";
        throw std::invalid_argument("Can't add cards to the trash pile!");
    }

    auto &pile = getMutable<TO>();
    if constexpr ( TO == shared::DRAW_PILE_TOP ) {
        pile.insert(pile.begin(), begin, end);
    } else {
        pile.insert(pile.end(), begin, end);
    }
}

template <enum shared::CardAccess TO>
inline void server::Player::add(std::vector<shared::CardBase::id_t> &&cards)
{
    add<TO>(std::make_move_iterator(cards.begin()), std::make_move_iterator(cards.end()));
}

template <enum shared::CardAccess TO>
inline void server::Player::add(const shared::CardBase::id_t &card_id)
{
    add<TO>(&card_id, &card_id + 1); // cursed lol
}

template <enum shared::CardAccess TO>
inline void server::Player::add(const std::vector<shared::CardBase::id_t> &cards)
{
    add<TO>(cards.begin(), cards.end());
}