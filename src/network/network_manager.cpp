#include <network/network_manager.hpp>

auto say_hi(const network::Datagram<> &data)
{
    std::cout << "hi\n";
}

auto print_num(const network::Datagram<int> &data)
{
    std::cout << data.get<0>() << std::endl;
    return network::Datagram<int>{data.get<0>()};
}

auto add(const network::Datagram<int, int> &data)
{
    std::cout << data.get<0>() << "+" << data.get<1>() << "\n";
    return network::Datagram<int>{data.get<0>() + data.get<1>()};
}

inline network::RPCChain<&print_num, &print_num, &print_num, &print_num, &print_num> add_rpc2;

network::NetworkManager::NetworkManager(Server &interface)
    : interface{interface}, id{0}, is_server{true}
{
    init();
}

void network::NetworkManager::setID(const unsigned int &id_)
{
    id = id_;
}

bool network::NetworkManager::isConnected() const
{
    return is_connected;
}

unsigned int network::NetworkManager::getID()
{
    return network_manager->id;
}
network::NetworkManager::NetworkManager(Client &interface)
    : interface{interface}, is_server{false}
{
    init();
}

network::RPC<network::Datagram<>, [](const auto &data)
             {
                 static unsigned int max_id = 1;
                 return network::Datagram{max_id++};
             },
             network::RPC<network::Datagram<unsigned int>, [](const auto &data)
                          {
                network::network_manager->setID(data.get());
                network::network_manager->setConnected();

                std::cout << "set ID to "<< data.get() << "\n"; }>>
    get_client_id;

void network::NetworkManager::connected()
{
    send(get_client_id, RPCTargets::Server, {});
}
void network::NetworkManager::setConnected(const bool &connected)
{
    is_connected = connected;
}

void network::NetworkManager::clientConnected(uv_stream_t *client)
{
    for (const auto &message : buffered_messages)
        interface.send(message, client);
}

void network::NetworkManager::connectionClosed()
{
    setConnected(false);
}

void network::NetworkManager::init()
{
    network_manager = this;
    interface.setCallbackReceive([this](const std::string_view &data, uv_stream_t *client)
                          { receive(data, client); });
    interface.setCallbackConnected([this]()
                          { connected(); });

    interface.setCallbackNewConnection([this](uv_stream_t *client)
                          { clientConnected(client); });

    interface.setCallbackConnectionLost([this]()
                          { connectionClosed(); });

    rpc_executor_thread = std::thread([&]()
                { executeRPCs(); });

    rpc_executor_thread.detach();
}

void network::NetworkManager::receive(const std::string_view &data, uv_stream_t *client)
{

    intptr_t client_ptr = reinterpret_cast<intptr_t>(client);

    ClientInfo &info = infos[client_ptr];
    info.append(data);
    info.client_stream = client;

    queue.push(&info);
    // queue_wait.notify_one();

    rpc_call_semaphore.release();
}

void network::NetworkManager::executeRPCs()
{
    while (true)
    {
        rpc_call_semaphore.acquire();
        // std::unique_lock<std::mutex> lock(queue_mutex);

        // queue_wait.wait(lock, [this]
        //                 { return !queue.empty(); });

        auto *client = queue.front();
        queue.pop();

        while (true)
        {
            const auto message = client->createMessage();
            if (!message)
                break;

            const RPCPacketHeader *header = (RPCPacketHeader *)((*message).data());
            const std::string_view view{(*message).data() + sizeof(RPCPacketHeader), (*message).size() - sizeof(RPCPacketHeader)};

            if (isClient() || (isServer() && header->target & RPCTargets::Server))
                executeRPC(*header, view, *client);

            if (isServer() && header->target & RPCTargets::Clients)
            {
                for (const auto &[ptr, data] : infos)
                {
                    auto client_ptr = reinterpret_cast<uv_stream_t *>(ptr);
                    if (client_ptr == client->client_stream)
                        continue;
                    interface.send(*message, client_ptr);
                }
            }

            if (isServer() && header->target & RPCTargets::Buffered)
            {
                buffered_messages.push_back(*message);
            }
        }
    }
}

void network::NetworkManager::executeRPC(const RPCPacketHeader &header, const std::string_view &data, const ClientInfo &client)
{
    assert(rpcs.contains(header.rpc_id));

    const auto result = rpcs[header.rpc_id]->rpc(data);
    if (!result)
        return;

    interface.send(*result, client.client_stream);
}

bool network::NetworkManager::isServer()
{
    return is_server;
}

bool network::NetworkManager::isClient()
{
    return !isServer();
}
