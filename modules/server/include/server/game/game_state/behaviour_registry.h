#pragma once

#include <memory>
#include <stdexcept>
#include <string>
#include <typeindex>
#include <unordered_map>
#include <vector>

#include <server/game/game_state/behaviour.h>
#include <shared/utils/utils.h>

namespace server
{
    class BehaviourBase; // forward declaration

    /**
     * @brief here we can look up card_ids/behaviour lists.
     * compiletime is not possible, so we will have to put it in its constructor.
     */
    class BehaviourRegistry
    {
    public:
        using map_t = std::unordered_map<std::string, std::vector<std::unique_ptr<BehaviourBase>>>;

        BehaviourRegistry();

        static bool has(const std::string &card_id) { return map_.count(card_id) > 0; }

        template <typename... BehaviourTypes>
        void insert(const std::string &card_id)
        {


            LOG(INFO) << "Registering card: " << card_id;
            ((LOG(INFO) << "  Behaviour type: " << utils::demangle(typeid(BehaviourTypes).name())), ...);

            (map_[card_id].emplace_back(std::make_unique<BehaviourTypes>()), ...);
        }

        const std::vector<std::unique_ptr<BehaviourBase>> &getBehaviours(const std::string &card_id);

    private:
        static map_t map_;
    };

} // namespace server