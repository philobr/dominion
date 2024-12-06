#include <server/game/behaviour_registry.h>

std::vector<std::unique_ptr<server::base::Behaviour>>
server::BehaviourRegistry::getBehaviours(const std::string &card_id)
{
    auto it = _map.find(card_id);
    if ( it == _map.end() ) {
        throw std::runtime_error("card_id not found in the registry");
    }
    return it->second();
}

server::BehaviourRegistry::BehaviourRegistry()
{
    if ( _is_initialised ) {
        LOG(INFO) << "BehaviourRegistry is already initialised, skipping card registration";
        return;
    }

    LOG(INFO) << "Initialising BehaviourRegistry";

    initialiseBehaviours();

    _is_initialised = true;
}

void server::BehaviourRegistry::initialiseBehaviours()
{
    using namespace server;
    using namespace behaviour;

    /**
     * behaviours are registered to card names here.
     * insert can take multiple types as template param
     */

    // ================================
    // TO BE DELETED: issue #165
    insert<GainCoins<1>>("Placeholder1");
    insert<GainCoins<1>>("Placeholder2");
    insert<GainCoins<1>>("Placeholder3");
    // ================================

    /*
    DONE
    */

    // treasure cards
    insert<GainCoins<1>>("Copper");
    insert<GainCoins<2>>("Silver");
    insert<GainCoins<3>>("Gold");

    // victory cards
    insert<GainPoints<1>>("Estate");
    insert<GainPoints<3>>("Duchy");
    insert<GainPoints<6>>("Province");
    insert<GainPoints<-1>>("Curse");

    // kingdom cards
    insert<DrawCards<2>>("Moat");
    insert<DrawCards<3>>("Smithy");
    insert<GainActions<2>, DrawCards<1>>("Village");
    insert<GainActions<1>, DrawCards<2>>("Laboratory");
    insert<GainActions<2>, GainCoins<2>, GainBuys<1>>("Festival");
    insert<GainActions<1>, GainCoins<1>, GainBuys<1>, DrawCards<1>>("Market");

    // enemies draw a card
    insert<DrawCards<4>, GainBuys<1>, DrawCardsEnemies<1>>("Council_Room");

    // enemies get curse on discard pile
    insert<DrawCards<2>, CurseEnemy>("Witch");

    /*
    UNSURE
     */
    // next action card counts double
    insert<NOT_IMPLEMENTED_YET>("Throne_Room"); // how 'sticky' card?
    // gain +1 coin if you play silver for the first time
    insert<DrawCards<1>, GainActions<1>, NOT_IMPLEMENTED_YET>("Merchant"); // how conditional?

    /*
    TODO:
    */
    // count points, only if game is over!
    insert<NOT_IMPLEMENTED_YET>("Gardens");
    // order to trash card, then actually trash the cards
    insert<NOT_IMPLEMENTED_YET>("Chapel");
    // look through discard pile, may put one card onto deck
    insert<DrawCards<1>, GainActions<1>, NOT_IMPLEMENTED_YET>("Harbinger");
    // gain as many cards as you have spent
    insert<GainActions<1>, NOT_IMPLEMENTED_YET>("Cellar");
    // discard a card per empty supply pile
    insert<DrawCards<1>, GainActions<1>, GainCoins<1>, NOT_IMPLEMENTED_YET>("Poacher");
    // enemies discard down to three
    insert<GainCoins<2>, NOT_IMPLEMENTED_YET>("Militia");
    // peek top 2 from deck, trash (and/or) discard any. return rest to draw pile in any order
    insert<DrawCards<1>, GainActions<1>, NOT_IMPLEMENTED_YET>("Sentry");

    // gain any card costing up to 4
    insert<NOT_IMPLEMENTED_YET>("Workshop");
    // discard top of draw pile, if action you may play it
    insert<NOT_IMPLEMENTED_YET>("Vassal");
    // may trash copper and gain +3
    insert<NOT_IMPLEMENTED_YET>("Moneylender");
    // compilcated, google it
    insert<NOT_IMPLEMENTED_YET>("Bureaucrat");
    // trash card from hand, gain card (to discard pile), costing 2 more than it
    insert<NOT_IMPLEMENTED_YET>("Remodel");
    // draw until 7 hand cards, skip action cards or keep (if skip then discard pile, if keep hand)
    insert<NOT_IMPLEMENTED_YET>("Library");
    // gain a gold, enemies reveal top two card from deck trash any treasure except copper, discard rest
    insert<NOT_IMPLEMENTED_YET>("Bandit");
    // may trash treasure from hand, gain treasure costing 3 more than it
    insert<NOT_IMPLEMENTED_YET>("Mine");
    // gain card costing up to 5. put card from hand onto deck
    insert<NOT_IMPLEMENTED_YET>("Artisan");
}
