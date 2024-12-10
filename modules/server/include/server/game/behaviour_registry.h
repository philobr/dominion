#pragma once

#include <functional>
#include <memory>
#include <stdexcept>
#include <string>
#include <typeindex>
#include <unordered_map>
#include <vector>

#include <server/game/behaviour_base.h>
#include <server/game/victory_card_behaviours.h>
#include <shared/game/cards/card_base.h>
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

        VictoryCardBehaviour &getVictoryBehaviour(const shared::CardBase::id_t &card_id) const;

    private:
        /**
         * @brief Initialises all behaviours for the the cards of the basegame 2nd edition.
         */
        void initialiseBehaviours();

        /**
         * @brief Inserts a list of behaviours into the registry. This will be called from the constructor.
         *
         * @tparam BehaviourType The behaviours that will be used for the card.
         */
        template <typename... BehaviourType>
        inline void insert(const shared::CardBase::id_t &card_id);

        /**
         * @brief Inserts a list of behaviours into the registry. This will be called from the constructor.
         *
         * @tparam VictoryCardBehaviour The behaviour that will be used for the victory points.
         * @tparam BehaviourTypes The behaviours that will be used for the card.
         */
        template <typename VictoryCardBehaviour, typename... BehaviourTypes>
        void insertVictory(const shared::CardBase::id_t &card_id);

        // TODO(#229): I guess this is a memory leak.
        // In order to fix this, we could make the behaviour registry a singleton.
        static std::unordered_map<shared::CardBase::id_t, std::unique_ptr<VictoryCardBehaviour>> _victory_map;
        static std::unordered_map<shared::CardBase::id_t,
                                  std::function<std::vector<std::unique_ptr<base::Behaviour>>()>>
                _map;
        static bool _is_initialised;
    };

    // static member initialisation
    inline std::unordered_map<shared::CardBase::id_t, std::unique_ptr<VictoryCardBehaviour>>
            BehaviourRegistry::_victory_map;
    inline std::unordered_map<std::string, std::function<std::vector<std::unique_ptr<base::Behaviour>>()>>
            BehaviourRegistry::_map;
    inline bool BehaviourRegistry::_is_initialised;

    template <typename... BehaviourType>
    inline void BehaviourRegistry::insert(const shared::CardBase::id_t &card_id)
    {
        insertVictory<ConstantVictoryPoints<0>, BehaviourType...>(card_id);
    }

    template <typename VictoryCardBehaviour, typename... BehaviourType>
    inline void BehaviourRegistry::insertVictory(const shared::CardBase::id_t &card_id)
    {
        LOG(INFO) << "Registering card: " << card_id;
        ((LOG(INFO) << "  Behaviour type: " << utils::demangle(typeid(BehaviourType).name())), ...);

        _victory_map[card_id] = std::make_unique<VictoryCardBehaviour>();

        _map[card_id] = []()
        {
            std::vector<std::unique_ptr<base::Behaviour>> behaviours;
            (behaviours.emplace_back(std::make_unique<BehaviourType>()), ...);
            return behaviours;
        };
    }
} // namespace server
