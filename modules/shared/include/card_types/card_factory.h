#pragma once

#include <memory>
#include <map>
#include <vector>

#include "types.h"

// forward declaration
class CardBase;

class CardFactory {
public:
    using CardPtr = std::unique_ptr<CardBase>;
    using MapType = std::map<card_id_t, CardPtr>;

    static CardBase* getCard(const card_id_t& id)
    {
        auto it = card_map_m.find(id);
        if ( it != card_map_m.end() ) {
            return it->second.get();
        }
        return nullptr;
    }

    static void registerCard(const card_id_t& id, CardPtr card)
    {
        card_map_m[id] = std::move(card);
    }

    static const MapType& getCardMap()
    {
        return card_map_m;
    }

private:
    static MapType card_map_m;
};

CardFactory::MapType CardFactory::card_map_m;