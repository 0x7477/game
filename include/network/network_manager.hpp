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

static inline network::RPC<network::Datagram<>,[](const network::Datagram<>& data){std::cout << "data.get<0>()" << "\n";}>get_client_id;

namespace network
{
    class NetworkManager
    {
    public:
        NetworkManager(Server &interface)
            : interface{interface}, id{0}
        {
            init();
            // std::cout<< get_client_id.getID() << "\n";

        }
        NetworkManager(Client &interface)
            : interface{interface}
        {
            // std::cout<< get_client_id.getID() << "\n";
            init();

        }

        void connected()
        {
            std::cout << "retrieve ID\n";
            send(get_client_id, RPCTarget::Server, {});
        }

        void init()
        {
            interface.setCallback([this](const std::string_view &data, const uv_stream_t *client){receive(data,client);});
            interface.setCallback([this](){connected();});
            std::thread([&](){createRPCs();}).detach();
        }

        void receive(const std::string_view &data, const uv_stream_t *client)
        {
            std::cout << "received data\n";
        
            intptr_t client_ptr = reinterpret_cast<intptr_t>(client);

            ClientInfo& info = infos[client_ptr];
            info.append(data);

            queue.push(&info);
            queue_wait.notify_one();
        }

        void createRPCs()
        {
            while(true)
            {
                std::unique_lock<std::mutex> lock(queue_mutex);
                queue_wait.wait(lock, [this] { return !queue.empty(); });

                auto* client = queue.front();
                queue.pop();

                while(true)
                {

                const auto message = client->createMessage();
                if(!message) break;


                RPCPacketHeader* header = (RPCPacketHeader*)((*message).data());

                const std::string_view view{(*message).data() + sizeof(RPCPacketHeader), (*message).size() - sizeof(RPCPacketHeader)};

                std::cout << "header->rpc_id " << header->rpc_id << "\n";
                if (rpcs.contains(header->rpc_id))
                    rpcs[header->rpc_id]->rpc(view);

                    }

            }
        }

        template <class datagram = Datagram<>, auto execution = [](const datagram &) {}>
        void send(const RPC<datagram, execution> &rpc, const RPCTarget::RPCTarget &target, const datagram &dgram)
        {
            interface.send(rpc.serialize(target, dgram));
        }

        bool isServer()
        {
            return id && *id==0;
        }

        bool isClient()
        {
            return !isServer();
        }

    private:

        std::condition_variable queue_wait;
        std::mutex queue_mutex;
        std::queue<ClientInfo*> queue;
        
        std::map<intptr_t, ClientInfo> infos; 
        NetworkInterface &interface;
        std::optional<unsigned int> id{};
    };

}

