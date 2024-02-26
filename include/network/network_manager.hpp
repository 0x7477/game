#pragma once
#include "server.hpp"
#include "client.hpp"
#include "client_info.hpp"
#include <optional>
#include "rpc.hpp"
#include <condition_variable>
#include <queue>
#include <algorithm>
#include <ranges>
#include "rpcs/echo.hpp"

namespace network
{
    class NetworkManager;
    inline NetworkManager *network_manager{nullptr};

    class NetworkManager
    {
    public:
        NetworkManager(Server &interface);
        NetworkManager(Client &interface);

        void setID(const unsigned int& id);
        unsigned int getID();

        void connected();

        void init();

        void receive(const std::string_view &data, uv_stream_t *client);

        void executeRPCs();

        template <class input = Datagram<>, auto execution = [](const input &) -> std::optional<SerializedDatagram>{ return {}; }, typename response = void>
        void send(const RPC<input, execution, response> &rpc, const RPCTarget::RPCTarget &target, const input &dgram)
        {
            const auto serialized {rpc.serialize(target, dgram)};
            interface.send(serialized);
        }

        bool isServer();
        bool isClient();

    private:
        std::condition_variable queue_wait;
        std::mutex queue_mutex;
        std::queue<ClientInfo *> queue;

        std::map<intptr_t, ClientInfo> infos;
        NetworkInterface &interface;
        unsigned int id{};
    };

}
