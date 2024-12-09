#pragma once

#include <server/network/basic_network.h>
#include <shared/message_types.h>

namespace server
{
    /**
     * @brief Abstract base class for sending messages to the client
     *
     * @details Used for enabling Mocking of the send_message method
     * for writing tests.
     */
    class MessageInterface
    {
    public:
        MessageInterface() = default;
        virtual ~MessageInterface() = default;
        /**
         * @brief gets a response that has to be sent to a client and passes it to the network manager as json string
         */
        virtual void sendMessage(const shared::ServerToClientMessage &message,
                                 const shared::PlayerBase::id_t &player_id) = 0;

        /**
         * @brief Sends a message of provided type to given player.
         *
         * @tparam T
         * @tparam Args
         * @param player_id
         * @param args
         */
        template <typename T, typename... Args>
        void send(const shared::PlayerBase::id_t &player_id, Args &&...args)
        {
            static_assert(std::is_base_of<shared::ServerToClientMessage, T>::value,
                          "T must derive from shared::ServerToClientMessage");
            T message(std::forward<Args>(args)...);
            sendMessage(message, player_id);
        }

        /**
         * @brief Broadcasts a message of given type to the given players
         */
        template <typename T, typename... Args>
        void broadcast(const std::vector<shared::PlayerBase::id_t> &players, Args &&...args)
        {
            static_assert(std::is_base_of<shared::ServerToClientMessage, T>::value,
                          "T must derive from shared::ServerToClientMessage");

            for ( const auto &player_id : players ) {
                T message(std::forward<Args>(args)...);
                sendMessage(message, player_id);
            }
        }
    };

    class ImplementedMessageInterface : public MessageInterface
    {
    public:
        ImplementedMessageInterface() = default;
        ~ImplementedMessageInterface() override = default;
        void sendMessage(const shared::ServerToClientMessage &message,
                         const shared::PlayerBase::id_t &player_id) override;
    };

} // namespace server
