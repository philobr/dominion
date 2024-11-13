#pragma once

#include <map>
#include <server/game/cards/cards_core/server_card.h>
namespace server
{
    /**
     * @brief This class manages and registers cards.
     * The cards get assigned to the map at compiletime and can be retrieved using CardFactory::getCard(card_id).
     * One can also get a reference to all registered cards.
     */
    class CardFactory
    {
    public:
        using key_t = server::CardBase::id_t;
        using val_t = server::CardBase::ptr_t;
        using map_t = std::map<key_t, val_t>;

        /**
         * @brief Returns a pointer to a card in the map, will be nullptr if the map does not contain the card
         *
         * @param id
         * @return server::CardBase*
         */
        static server::CardBase *getCard(const key_t &id)
        {
            auto it = card_map_m.find(id);
            if ( it != card_map_m.end() ) {
                return it->second.get();
            }
            return nullptr;
        }

        /**
         * @brief registers a card based on key and value
         *
         * @param id    server::CardBase::id_t
         * @param card  server::CardBase::ptr_t
         */
        static void registerCard(const key_t &id, val_t card) { card_map_m[id] = std::move(card); }

        /**
         * @brief Get the card map
         *
         * @return const map_t&
         */
        static const map_t &getCardMap() { return card_map_m; }

    private:
        static map_t card_map_m;
    };
} // namespace server