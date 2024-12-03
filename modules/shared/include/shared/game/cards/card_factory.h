#pragma once

#include <unordered_map>

#include <shared/game/cards/card_base.h>

namespace shared
{
    class CardFactory
    {
    public:
        using map_t = std::unordered_map<CardBase::id_t, std::unique_ptr<CardBase>>;

        static void insert(const CardBase::id_t &card_id, CardType type, unsigned int cost)
        {
            _map.emplace(card_id, std::make_unique<CardBase>(card_id, type, cost));
        }

        static const CardBase &getCard(const CardBase::id_t &card_id) { return *_map.at(card_id).get(); }
        static const map_t &getAll() { return _map; }

        static bool has(const CardBase::id_t &card_id) { return _map.count(card_id) > 0; }

    private:
        static map_t _map;
    };

} // namespace shared