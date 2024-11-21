#pragma once

#include <memory>
#include <stdexcept>
#include <string>
#include <typeindex>
#include <unordered_map>
#include <vector>

#include "behaviour.h"

class BehaviourRegistry
{
    std::unordered_map<std::string, std::vector<std::unique_ptr<BehaviourBase>>> map_;

public:
    BehaviourRegistry() { insert<GainCoins<3>, GainCoins<2>, GainCoins<1>>("Laboratory"); }

    template <typename... BehaviourTypes>
    void insert(const std::string &card_id)
    {
        if ( map_.find(card_id) == map_.end() ) {
            map_[card_id] = {};
        }

        (map_[card_id].push_back(std::make_unique<BehaviourTypes>()), ...);
    }

    const std::vector<std::unique_ptr<BehaviourBase>> &getBehaviours(const std::string &card_id)
    {
        auto it = map_.find(card_id);
        if ( it == map_.end() ) {
            throw std::runtime_error("card_id not found in the registry");
        }
        return it->second;
    }
};