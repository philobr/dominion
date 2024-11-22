#include <shared/game/cards/card_factory.h>

namespace shared
{
    // static member declaration
    CardFactory::map_t CardFactory::map_;

    // idk if its actually called registrar, sounds coll though, just a helper struct
    struct CardRegistrar
    {
        CardRegistrar(const CardBase::id_t &card_id, CardType type, unsigned int cost)
        {
            CardFactory::insert(card_id, type, cost);
        }
    };

#define REGISTER_CARD(card_id, card_type, card_cost)                                                                   \
    static shared::CardRegistrar card_registrar_##card_id(#card_id, static_cast<CardType>(card_type), card_cost)

    /*
    SUPPLY CARDS
     */

    // treasure
    REGISTER_CARD(Copper, CardType::TREASURE, 0);
    REGISTER_CARD(Silver, CardType::TREASURE, 2);
    REGISTER_CARD(Gold, CardType::TREASURE, 5);

    // victory
    REGISTER_CARD(Estate, CardType::VICTORY, 2);
    REGISTER_CARD(Duchy, CardType::VICTORY, 5);
    REGISTER_CARD(Province, CardType::VICTORY, 8);

    // curse
    REGISTER_CARD(Curse, CardType::CURSE, 0);

    /*
    KINGDOM CARDS
    */

    REGISTER_CARD(Merchant, CardType::ACTION, 3); // conditional effect, how?
    REGISTER_CARD(Throne_Room, CardType::ACTION, 4); // how do we keep this active?

    // non-interactive
    REGISTER_CARD(Moat, CardType::ACTION | CardType::REACTION, 2);
    REGISTER_CARD(Village, CardType::ACTION, 3);
    REGISTER_CARD(Smithy, CardType::ACTION, 4);
    REGISTER_CARD(Gardens, CardType::ACTION | CardType::VICTORY, 4);
    REGISTER_CARD(Festival, CardType::ACTION, 5);
    REGISTER_CARD(Market, CardType::ACTION, 5);
    REGISTER_CARD(Laboratory, CardType::ACTION, 5);
    REGISTER_CARD(Council_Room, CardType::ACTION, 5);
    REGISTER_CARD(Witch, CardType::ACTION | CardType::ATTACK, 5);

    // interactive
    REGISTER_CARD(Cellar, CardType::ACTION, 2);
    REGISTER_CARD(Chapel, CardType::ACTION, 2);
    REGISTER_CARD(Workshop, CardType::ACTION, 3);
    REGISTER_CARD(Vassal, CardType::ACTION, 3);
    REGISTER_CARD(Harbinger, CardType::ACTION, 3);
    REGISTER_CARD(Poacher, CardType::ACTION, 4);
    REGISTER_CARD(Moneylender, CardType::ACTION, 4);
    REGISTER_CARD(Militia, CardType::ACTION | CardType::ATTACK, 4);
    REGISTER_CARD(Bureaucrat, CardType::ACTION | CardType::ATTACK, 4);
    REGISTER_CARD(Remodel, CardType::ACTION, 4);
    REGISTER_CARD(Sentry, CardType::ACTION, 5);
    REGISTER_CARD(Library, CardType::ACTION, 5);
    REGISTER_CARD(Bandit, CardType::ACTION | CardType::ATTACK, 5);
    REGISTER_CARD(Mine, CardType::ACTION, 5);
    REGISTER_CARD(Artisan, CardType::ACTION, 6);

#undef REGISTER_CARD
} // namespace shared
