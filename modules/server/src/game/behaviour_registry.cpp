
#include <server/debug_mode.h>
#include <server/game/behaviour_registry.h>
#include <server/game/victory_card_behaviours.h>
#include <shared/game/cards/card_factory.h>

std::vector<std::unique_ptr<server::base::Behaviour>>
server::BehaviourRegistry::getBehaviours(const std::string &card_id)
{
    auto it = _map.find(card_id);
    if ( it == _map.end() ) {
        LOG(ERROR) << "Requested card \'" << card_id << "\' not registered in the BehaviourRegistry!";
        throw exception::CardNotAvailable("card not found: " + card_id);
    }
    return it->second();
}

server::VictoryCardBehaviour &
server::BehaviourRegistry::getVictoryBehaviour(const shared::CardBase::id_t &card_id) const
{
    auto it = _victory_map.find(card_id);
    if ( it == _victory_map.end() ) {
        LOG(WARN) << "Requested victory card \'" << card_id << "\' not registered in the BehaviourRegistry!";
        throw exception::CardNotAvailable("Requested card not found in the victory card registry: " + card_id);
    }
    return *it->second;
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

    /**
     * If we are running in debug mode, we enable "God Mode".
     */
    if ( DEBUG_MODE ) {
        insert<GainCoins<1000>, GainActions<1000>, GainBuys<1000>>("God_Mode");
    }

    /*
    DONE
    */

    // treasure cards
    insert<GainCoins<1>>("Copper");
    insert<GainCoins<2>>("Silver");
    insert<GainCoins<3>>("Gold");

    // victory cards
    insertVictory<ConstantVictoryPoints<1>>("Estate");
    insertVictory<ConstantVictoryPoints<3>>("Duchy");
    insertVictory<ConstantVictoryPoints<6>>("Province");
    insertVictory<ConstantVictoryPoints<-1>>("Curse");

    // kingdom cards
    insert<DrawCards<3>>("Smithy");
    insert<GainActions<2>, DrawCards<1>>("Village");
    insert<GainActions<1>, DrawCards<2>>("Laboratory");
    insert<GainActions<2>, GainCoins<2>, GainBuys<1>>("Festival");
    insert<GainActions<1>, GainCoins<1>, GainBuys<1>, DrawCards<1>>("Market");

    insert<DrawCards<1>, GainActions<2>, GainBuys<1>>("Workers_Village");
    insert<SeaHag>("Sea_Hag");
    insertVictory<ConstantVictoryPoints<1>, DrawCards<1>, GainActions<1>>("Great_Hall");
    insert<TreasureMap>("Treasure_Map");

    // money cards
    insert<GainCoins<2>, TreasureTrove>("Treasure_Trove");

    // enemies draw a card
    insert<DrawCards<4>, GainBuys<1>, DrawCardsEnemies<1>>("Council_Room");
    // may trash copper and gain +3
    insert<Moneylender>("Moneylender");
    // enemies get curse on discard pile
    insert<DrawCards<2>, CurseEnemy>("Witch");
    // gain card costing up to 5. put card from hand onto deck
    insert<GainCardMaxCost<5>, CardToDrawPile>("Artisan");
    // may trash treasure from hand, gain treasure costing 3 more than it
    insert<Mine>("Mine");
    // order to trash card, then actually trash the cards
    insert<TrashCards<4>>("Chapel");
    // gain as many cards as you have spent
    insert<GainActions<1>, DiscardAndRedrawAnyAmount>("Cellar");
    // draw to cards and you can block enemy attacks
    insert<DrawCards<2>>("Moat");
    // gain any card costing up to 4
    insert<GainCardMaxCost<4>>("Workshop");

    auto gardens_filter = [](const shared::CardBase::id_t & /*card*/) -> bool { return true; };
    insertVictory<VictoryPointsPerNCards<1, 10, gardens_filter>>("Gardens");

    auto duke_filter = [](const shared::CardBase::id_t &card) -> bool { return card == "Duchy"; };
    insertVictory<VictoryPointsPerNCards<1, 1, duke_filter>>("Duke");

    auto silk_road_filter = [](const shared::CardBase::id_t &card) -> bool
    { return shared::CardFactory::isVictory(card); };
    insertVictory<VictoryPointsPerNCards<1, 4, silk_road_filter>>("Silk_Road");


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

    // look through discard pile, may put one card onto deck
    insert<DrawCards<1>, GainActions<1>, NOT_IMPLEMENTED_YET>("Harbinger");
    // discard a card per empty supply pile
    insert<DrawCards<1>, GainActions<1>, GainCoins<1>, NOT_IMPLEMENTED_YET>("Poacher");
    // enemies discard down to three
    insert<GainCoins<2>, MilitiaAttack>("Militia");
    // peek top 2 from deck, trash (and/or) discard any. return rest to draw pile in any order
    insert<DrawCards<1>, GainActions<1>, NOT_IMPLEMENTED_YET>("Sentry");
    // discard top of draw pile, if action you may play it
    insert<NOT_IMPLEMENTED_YET>("Vassal");
    // compilcated, google it
    insert<NOT_IMPLEMENTED_YET>("Bureaucrat");
    // trash card from hand, gain card (to discard pile), costing 2 more than it
    insert<NOT_IMPLEMENTED_YET>("Remodel");
    // draw until 7 hand cards, skip action cards or keep (if skip then discard pile, if keep hand)
    insert<NOT_IMPLEMENTED_YET>("Library");
    // gain a gold, enemies reveal top two card from deck trash any treasure except copper, discard rest
    insert<NOT_IMPLEMENTED_YET>("Bandit");
}
