#include <server/network/basic_network.h>

namespace server{

        ssize_t BasicNetwork::send_message(std::string &message, std::string &address){

            _rw_lock.lock();

            std::stringstream ss_msg;
            ss_msg << std::to_string(message.size()) << ':' << message; // prepend message length
            ssize_t ret = _address_to_socket.at(address).write(ss_msg.str());

            _rw_lock.unlock();
            return ret;
        }
        
        void BasicNetwork::add_player_to_address(player_id_t &player_id, std::string & address){
            _rw_lock.lock_shared();
            if(is_new_player(player_id)){
                // save connection to this client
                _rw_lock.unlock_shared();
                std::cout << "New client with id " << player_id << std::endl;
                _rw_lock.lock();
                _player_id_to_address.emplace(player_id, address);
                _rw_lock.unlock();
            }
        }

        void BasicNetwork::add_address_to_socket(std::string &address, sockpp::tcp_socket socket){
            _rw_lock.lock();
            _address_to_socket.emplace(address, std::move(socket.clone()));
            _rw_lock.unlock();
        }

        bool BasicNetwork::is_new_player(player_id_t &player_id){
            return _player_id_to_address.find(player_id) == _player_id_to_address.end(); 
        }

        std::string BasicNetwork::get_address(const player_id_t &player_id){
            if(_player_id_to_address.find(player_id) != _player_id_to_address.end()){
                return _player_id_to_address[player_id];
            }
        }

        void BasicNetwork::player_disconnect(std::string player_id){
            _rw_lock.lock();
            std::string address = _player_id_to_address[player_id];
            _player_id_to_address.erase(player_id);
            _address_to_socket.erase(address);
            _rw_lock.unlock();
        }
} // namespace server