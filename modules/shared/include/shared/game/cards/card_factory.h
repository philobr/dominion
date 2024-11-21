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
            map_.emplace(card_id, std::make_unique<CardBase>(card_id, type, cost));
        }

        static const CardBase &get(const CardBase::id_t &card_id) { return *map_.at(card_id).get(); }
        static const map_t &getAll() { return map_; }

        static bool has(const CardBase::id_t &card_id) { return map_.count(card_id) > 0; }

    private:
        static map_t map_;
    };

} // namespace shared