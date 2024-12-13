#include "server_player.h"

#pragma region UTILS
template <enum shared::CardAccess PILE>
inline std::vector<shared::CardBase::id_t> &server::Player::getMutable()
{
    static_assert(PILE != shared::TRASH && "Player does not have access to the trash pile!");
    if constexpr ( PILE == shared::DISCARD_PILE ) {
        return discard_pile;
    } else if constexpr ( PILE == shared::HAND ) {
        return hand_cards;
    } else if constexpr ( PILE == shared::STAGED_CARDS ) {
        return staged_cards;
    } else if constexpr ( PILE == shared::DRAW_PILE_TOP ) {
        return draw_pile;
    } else if constexpr ( PILE == shared::DRAW_PILE_BOTTOM ) {
        return draw_pile;
    } else {
        // should only happen for the trash pile i.e. never, as we assert at compiletime
        LOG(ERROR) << "Invalid pile specified or pile is not accessible.";
        throw std::invalid_argument("Invalid pile specified or pile is not accessible.");
    }
}

template <enum shared::CardAccess PILE>
inline const std::vector<shared::CardBase::id_t> &server::Player::get() const
{
    static_assert(PILE != shared::TRASH && "Player does not have access to the trash pile!");

    if constexpr ( PILE == shared::DISCARD_PILE ) {
        return discard_pile;
    } else if constexpr ( PILE == shared::HAND ) {
        return hand_cards;
    } else if constexpr ( PILE == shared::STAGED_CARDS ) {
        return staged_cards;
    } else if constexpr ( PILE == shared::DRAW_PILE_TOP ) {
        return draw_pile;
    } else if constexpr ( PILE == shared::DRAW_PILE_BOTTOM ) {
        return draw_pile;
    } else {
        // should only happen for the trash pile i.e. never, as we assert at compiletime
        LOG(ERROR) << "Invalid pile specified or pile is not accessible.";
        throw std::invalid_argument("Invalid pile specified or pile is not accessible.");
    }
}

template <enum shared::CardAccess PILE>
inline void server::Player::shuffle()
{
    static std::random_device rd;
    static std::mt19937 gen(rd());

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
inline bool server::Player::hasType(shared::CardType type) const
{
    auto pile = get<PILE>();
    return std::any_of(pile.begin(), pile.end(), [type](const auto &card_id)
                       { return (shared::CardFactory::getCard(card_id).getType() & type) == type; });
}

template <enum shared::CardAccess PILE>
inline std::vector<shared::CardBase::id_t> server::Player::getType(shared::CardType type) const
{
    auto pile = get<PILE>();
    std::vector<shared::CardBase::id_t> cards;
    std::copy_if(pile.begin(), pile.end(), std::back_inserter(cards),
                 [type](const auto &card_id) { return (shared::CardFactory::getCard(card_id).getType() & type) != 0; });
    return cards;
}

#pragma region ADD

template <enum shared::CardAccess TO, typename Iterator>
inline void server::Player::add(Iterator begin, Iterator end)
{
    static_assert(TO != shared::TRASH && "Can't add cards to the trash pile!");

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

#pragma region MOVE

template <enum shared::CardAccess FROM, enum shared::CardAccess TO>
inline void server::Player::move(const shared::CardBase::id_t &card_id)
{
    if constexpr ( TO == shared::TRASH ) {
        take<FROM>(card_id);
    } else {
        add<TO>(take<FROM>(card_id));
    }
}

template <enum shared::CardAccess FROM, enum shared::CardAccess TO>
inline void server::Player::move(const std::vector<shared::CardBase::id_t> &cards)
{
    if ( cards.empty() ) {
        LOG(WARN) << "Tried to move an empty set of cards from " << toString(FROM) << " to " << toString(TO);
        return;
    }

    std::for_each(cards.begin(), cards.end(), [this](const auto &card_id) { this->move<FROM, TO>(card_id); });
}

template <enum shared::CardAccess FROM, enum shared::CardAccess TO>
inline void server::Player::move(unsigned int n)
{
    if constexpr ( TO == shared::TRASH ) {
        take<FROM>(n);
    } else {
        add<TO>(take<FROM>(n));
    }
}

#pragma region TAKE

template <enum shared::CardAccess FROM>
inline shared::CardBase::id_t server::Player::take(const shared::CardBase::id_t &card_id)
{
    static_assert(FROM != shared::TRASH && "Can not take cards from the trash pile!");
    static_assert((FROM != shared::DRAW_PILE_TOP && FROM != shared::DRAW_PILE_BOTTOM) &&
                  "Can not take card from the draw pile by ID!");

    auto &pile = getMutable<FROM>();
    auto it = std::find(pile.begin(), pile.end(), card_id);

    if ( it == pile.end() ) {
        LOG(ERROR) << "Card \'" << card_id << "\' does not exist in the pile " << toString(FROM);
        throw exception::InvalidCardAccess();
    }

    pile.erase(it);
    return card_id;
}

template <enum shared::CardAccess FROM>
inline std::vector<shared::CardBase::id_t> server::Player::take(const std::vector<shared::CardBase::id_t> &cards)
{
    std::vector<shared::CardBase::id_t> taken_cards;
    std::for_each(cards.begin(), cards.end(),
                  [&taken_cards, this](const auto &card_id) { taken_cards.push_back(take<FROM>(card_id)); });
    return taken_cards;
}

template <enum shared::CardAccess FROM>
inline std::vector<shared::CardBase::id_t> server::Player::take(unsigned int n)
{
    auto &pile = getMutable<FROM>();

    if ( n == 0 ) {
        n = pile.size();
    }

    if constexpr ( FROM == shared::DRAW_PILE_TOP || FROM == shared::DRAW_PILE_BOTTOM ) {
        // special case: we try to move discard pile to bottom of draw_pile
        if ( pile.size() < n && !getMutable<shared::DISCARD_PILE>().empty() ) {
            shuffle<shared::DISCARD_PILE>();
            move<shared::DISCARD_PILE, shared::DRAW_PILE_BOTTOM>();
        }
        n = std::min(n, static_cast<unsigned int>(pile.size()));
    } else {
        // throw if we dont have enough cards
        if ( pile.size() < n ) {
            throw exception::WrongCardCount("Not enough cards in the pile to take.");
        }
    }

    std::vector<shared::CardBase::id_t> taken_cards;
    taken_cards.reserve(n);

    if constexpr ( FROM == shared::DRAW_PILE_TOP ) {
        // take from top
        taken_cards.assign(std::make_move_iterator(pile.begin()), std::make_move_iterator(pile.begin() + n));
        pile.erase(pile.begin(), pile.begin() + n);
    } else {
        // take from back
        taken_cards.assign(std::make_move_iterator(pile.end() - n), std::make_move_iterator(pile.end()));
        pile.erase(pile.end() - n, pile.end());
    }

    return taken_cards;
}