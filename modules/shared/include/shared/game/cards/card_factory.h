#pragma once

#include <map>
#include <unordered_map>

#include <shared/game/cards/card_base.h>
#include <shared/utils/logger.h>

namespace shared
{
    class CardFactory
    {
    public:
        using map_t = std::unordered_map<CardBase::id_t, std::unique_ptr<CardBase>>;
        // Greater bc the first element inserted in the view will be the lowest on the screen
        using sorted_t = std::multimap<unsigned int, std::unique_ptr<CardBase>, std::greater<>>;

        static void insert(const CardBase::id_t &card_id, CardType type, unsigned int cost);
        static bool has(const CardBase::id_t &card_id) { return _map.count(card_id) > 0; }

        static const map_t &getAll() { return _map; }
        static const sorted_t getAllSortedByCost();

        static const CardBase &getCard(const CardBase::id_t &card_id);
        static unsigned int getCost(const CardBase::id_t &card_id);
        static CardType getType(const CardBase::id_t &card_id);
        static CardBase::id_t getId(const CardBase::id_t &card_id);

        static bool isAction(const CardBase::id_t &card_id);
        static bool isAttack(const CardBase::id_t &card_id);
        static bool isReaction(const CardBase::id_t &card_id);
        static bool isTreasure(const CardBase::id_t &card_id);
        static bool isVictory(const CardBase::id_t &card_id);
        static bool isCurse(const CardBase::id_t &card_id);

    private:
        static map_t _map;
    };

} // namespace shared

// function implementations
namespace shared
{
    inline void shared::CardFactory::insert(const shared::CardBase::id_t &card_id, shared::CardType type,
                                            unsigned int cost)
    {
        _map.emplace(card_id, std::make_unique<CardBase>(card_id, type, cost));
    }

    inline const shared::CardBase &shared::CardFactory::getCard(const shared::CardBase::id_t &card_id)
    {
        return *_map.at(card_id).get();
    }

    inline unsigned int shared::CardFactory::getCost(const shared::CardBase::id_t &card_id)
    {
        if ( !has(card_id) ) {
            LOG(ERROR) << "Tried to access card: " << card_id << ", but this card does not exist";
            throw std::invalid_argument("card_id: " + card_id + " does not exist");
        }

        return CardFactory::getCard(card_id).getCost();
    }

    inline shared::CardType shared::CardFactory::getType(const shared::CardBase::id_t &card_id)
    {
        if ( !has(card_id) ) {
            LOG(ERROR) << "Tried to access card type for: " << card_id << ", but this card does not exist";
            throw std::invalid_argument("card_id: " + card_id + " does not exist");
        }
        return getCard(card_id).getType();
    }

    inline shared::CardBase::id_t shared::CardFactory::getId(const shared::CardBase::id_t &card_id)
    {
        if ( !has(card_id) ) {
            LOG(ERROR) << "Tried to access card ID for: " << card_id << ", but this card does not exist";
            throw std::invalid_argument("card_id: " + card_id + " does not exist");
        }
        return getCard(card_id).getId();
    }

    inline bool shared::CardFactory::isAction(const shared::CardBase::id_t &card_id)
    {
        if ( !has(card_id) ) {
            LOG(ERROR) << "Tried to check if card is action: " << card_id << ", but this card does not exist";
            throw std::invalid_argument("card_id: " + card_id + " does not exist");
        }
        return getCard(card_id).isAction();
    }

    inline bool shared::CardFactory::isAttack(const shared::CardBase::id_t &card_id)
    {
        if ( !has(card_id) ) {
            LOG(ERROR) << "Tried to check if card is attack: " << card_id << ", but this card does not exist";
            throw std::invalid_argument("card_id: " + card_id + " does not exist");
        }
        return getCard(card_id).isAttack();
    }

    inline bool shared::CardFactory::isReaction(const shared::CardBase::id_t &card_id)
    {
        if ( !has(card_id) ) {
            LOG(ERROR) << "Tried to check if card is reaction: " << card_id << ", but this card does not exist";
            throw std::invalid_argument("card_id: " + card_id + " does not exist");
        }
        return getCard(card_id).isReaction();
    }

    inline bool shared::CardFactory::isTreasure(const shared::CardBase::id_t &card_id)
    {
        if ( !has(card_id) ) {
            LOG(ERROR) << "Tried to check if card is treasure: " << card_id << ", but this card does not exist";
            throw std::invalid_argument("card_id: " + card_id + " does not exist");
        }
        return getCard(card_id).isTreasure();
    }

    inline bool shared::CardFactory::isVictory(const shared::CardBase::id_t &card_id)
    {
        if ( !has(card_id) ) {
            LOG(ERROR) << "Tried to check if card is victory: " << card_id << ", but this card does not exist";
            throw std::invalid_argument("card_id: " + card_id + " does not exist");
        }
        return getCard(card_id).isVictory();
    }

    inline bool shared::CardFactory::isCurse(const shared::CardBase::id_t &card_id)
    {
        if ( !has(card_id) ) {
            LOG(ERROR) << "Tried to check if card is curse: " << card_id << ", but this card does not exist";
            throw std::invalid_argument("card_id: " + card_id + " does not exist");
        }
        return getCard(card_id).isCurse();
    }

    inline const shared::CardFactory::sorted_t shared::CardFactory::getAllSortedByCost()
    {
        shared::CardFactory::sorted_t sorted_map;
        for ( const auto &card : _map ) {
            sorted_map.emplace(card.second->getCost(), card.second.get());
        }
        return sorted_map;
    }

} // namespace shared
