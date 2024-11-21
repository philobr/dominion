#include <server/game/game_state/behaviour_registry.h>


// static member declaration
server::BehaviourRegistry::map_t server::BehaviourRegistry::map_;

const std::vector<std::unique_ptr<server::BehaviourBase>> &
server::BehaviourRegistry::getBehaviours(const std::string &card_id)
{
    auto it = map_.find(card_id);
    if ( it == map_.end() ) {
        throw std::runtime_error("card_id not found in the registry");
    }
    return it->second;
}

server::BehaviourRegistry::BehaviourRegistry()
{
    LOG(INFO) << "Creating Behaviour registry";

    /**
     * behaviours are registered to card names here.
     * insert can take multiple types as template param
     */

    using namespace server;
    using namespace behaviour;

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
    insert<GainCoins<-1>>("Curse");

    insert<NOT_IMPLEMENTED_YET>("Merchant");
    insert<NOT_IMPLEMENTED_YET>("Throne_Room");
    insert<NOT_IMPLEMENTED_YET>("Moat");
    insert<NOT_IMPLEMENTED_YET>("Village");
    insert<NOT_IMPLEMENTED_YET>("Smithy");
    insert<NOT_IMPLEMENTED_YET>("Gardens");
    insert<NOT_IMPLEMENTED_YET>("Festival");
    insert<NOT_IMPLEMENTED_YET>("Market");
    insert<NOT_IMPLEMENTED_YET>("Laboratory");
    insert<NOT_IMPLEMENTED_YET>("Council_Room");
    insert<NOT_IMPLEMENTED_YET>("Witch");
    insert<NOT_IMPLEMENTED_YET>("Cellar");
    insert<NOT_IMPLEMENTED_YET>("Chapel");
    insert<NOT_IMPLEMENTED_YET>("Workshop");
    insert<NOT_IMPLEMENTED_YET>("Vassal");
    insert<NOT_IMPLEMENTED_YET>("Harbinger");
    insert<NOT_IMPLEMENTED_YET>("Poacher");
    insert<NOT_IMPLEMENTED_YET>("Moneylender");
    insert<NOT_IMPLEMENTED_YET>("Militia");
    insert<NOT_IMPLEMENTED_YET>("Bureaucrat");
    insert<NOT_IMPLEMENTED_YET>("Remodel");
    insert<NOT_IMPLEMENTED_YET>("Sentry");
    insert<NOT_IMPLEMENTED_YET>("Library");
    insert<NOT_IMPLEMENTED_YET>("Bandit");
    insert<NOT_IMPLEMENTED_YET>("Mine");
    insert<NOT_IMPLEMENTED_YET>("Artisan");
}