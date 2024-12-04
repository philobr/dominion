#pragma once

#include <functional>
#include <memory>
#include <stdexcept>
#include <string>
#include <typeindex>
#include <unordered_map>
#include <vector>

#include <server/game/behaviour_base.h>
#include <shared/utils/utils.h>

namespace server
{
    class BehaviourRegistry
    {
    public:
        /**
         * @brief Constructs a new BehaviourRegistry.
         * IF the behaviours are not already initialised it will do so.
         */
        BehaviourRegistry();

        std::vector<std::unique_ptr<base::Behaviour>> getBehaviours(const std::string &card_id);

    private:
        /**
         * @brief Initialises all behaviours for the the cards of the basegame 2nd edition.
         */
        void initialiseBehaviours();

        /**
         * @brief Inserts a list of behaviours into the registry. This will be called from the constructor.
         *
         * @tparam BehaviourTypes
         * @param card_id
         */
        template <typename... BehaviourTypes>
        void insert(const std::string &card_id);

        static std::unordered_map<std::string, std::function<std::vector<std::unique_ptr<base::Behaviour>>()>> _map;
        static bool _is_initialised;
    };

    // static member initialisation
    inline std::unordered_map<std::string, std::function<std::vector<std::unique_ptr<base::Behaviour>>()>>
            BehaviourRegistry::_map;
    inline bool BehaviourRegistry::_is_initialised;

    template <typename... BehaviourType>
    inline void BehaviourRegistry::insert(const std::string &card_id)
    {
        LOG(INFO) << "Registering card: " << card_id;
        ((LOG(INFO) << "  Behaviour type: " << utils::demangle(typeid(BehaviourType).name())), ...);

        _map[card_id] = []()
        {
            std::vector<std::unique_ptr<base::Behaviour>> behaviours;
            (behaviours.emplace_back(std::make_unique<BehaviourType>()), ...);
            return behaviours;
        };
    }
} // namespace server