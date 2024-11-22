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
        /**
         * @brief Constructs a new BehaviourRegistry.
         * IF the behaviours are not already initialised it will do so.
         */
        BehaviourRegistry();

        const std::vector<std::unique_ptr<BehaviourBase>> &getBehaviours(const std::string &card_id);

    private:
        /**
         * @brief Initialises all behaviours for the the cards of the basegame 2nd edition.
         */
        void initialise_behaviours();

        /**
         * @brief Inserts a list of behaviours into the registry. This will be called from the constructor.
         *
         * @tparam BehaviourTypes
         * @param card_id
         */
        template <typename... BehaviourTypes>
        void insert(const std::string &card_id);

        static std::unordered_map<std::string, std::vector<std::unique_ptr<BehaviourBase>>> map_;
        static bool is_initialised;
    };

    // static member initialisation
    inline std::unordered_map<std::string, std::vector<std::unique_ptr<BehaviourBase>>> server::BehaviourRegistry::map_;
    inline bool BehaviourRegistry::is_initialised;

    template <typename... BehaviourTypes>
    inline void BehaviourRegistry::insert(const std::string &card_id)
    {
        LOG(INFO) << "Registering card: " << card_id;
        ((LOG(INFO) << "  Behaviour type: " << utils::demangle(typeid(BehaviourTypes).name())), ...);

        (map_[card_id].emplace_back(std::make_unique<BehaviourTypes>()), ...);
    }
} // namespace server