#pragma once
#include "server.hpp"
#include "client.hpp"
#include "client_info.hpp"
#include <optional>
#include "rpc.hpp"
#include <condition_variable>
#include <queue>
#include <forward_list>
#include <algorithm>
#include <ranges>

#include <semaphore>

namespace network
{
    class NetworkManager;
    inline NetworkManager *network_manager{nullptr};

    class NetworkManager
    {
    public:
        NetworkManager(Server &interface);
        NetworkManager(Client &interface);

        ~NetworkManager()
        {
            is_running = false;

        }

        static unsigned int getID();

        void connected();

        void init();

        void setConnected(const bool &connected = true);

        bool isConnected() const;

        void receive(const std::string_view &data, uv_stream_t *client);

        void executeRPCs();
        void executeRPC(const RPCPacketHeader &header, const std::string_view &data, const ClientInfo &client);
        void clientConnected(uv_stream_t *client);
        void connectionClosed();

        template <class input = Datagram<>, auto execution = []() {}, typename response = void>
        static void send(RPC<input, execution, response> &rpc, const RPCTarget &target, const input &dgram)
        {
            const auto serialized{rpc.serialize(target, dgram)};
            if (network_manager->isClient())
            {
                if (target & RPCTargets::Me)
                    rpc.local_rpc(dgram);

                if (target & RPCTargets::Server || target & RPCTargets::Clients)
                    network_manager->interface.send(serialized);
            }
            else
            {
                if (target & RPCTargets::Server)
                    rpc.local_rpc(dgram);

                if (target & RPCTargets::Clients)
                {
                    for (const auto &[ptr, data] : network_manager->infos)
                    {
                        auto client_ptr = reinterpret_cast<uv_stream_t *>(ptr);
                        network_manager->interface.sendTo(serialized, client_ptr);
                    }
                }
            }
        }

        bool isServer();
        bool isClient();
        void setID(const unsigned int &id);

    private:

        std::counting_semaphore<1024> rpc_call_semaphore{0};
        // std::condition_variable queue_wait;
        // std::mutex queue_mutex;
        std::queue<ClientInfo *> queue;
        std::list<std::string> buffered_messages{};
        std::thread rpc_executor_thread;
        std::map<intptr_t, ClientInfo> infos;
        NetworkInterface &interface;
        unsigned int id{};
        bool is_server{};
        bool is_running{true};
        bool is_connected{false};
    };

}
