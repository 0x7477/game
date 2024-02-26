#include <network/network_manager.hpp>

network::RPC<network::Datagram<>, [](const network::Datagram<> &data)
             {
                 static unsigned int max_id = 1;
                 return network::Datagram{max_id++};
             },
             network::RPC<network::Datagram<unsigned int>, [](const network::Datagram<unsigned int> &data)
                          {
                network::network_manager->setID(data.get());
                std::cout << "set ID to "<< data.get() << "\n"; }>>
    get_client_id;

network::NetworkManager::NetworkManager(Server &interface)
    : interface{interface}, id{0}
{
    init();
    // std::cout<< get_client_id.getID() << "\n";
}

void network::NetworkManager::setID(const unsigned int &id_)
{
    id = id_;
}

unsigned int network::NetworkManager::getID()
{
    return id;
}
network::NetworkManager::NetworkManager(Client &interface)
    : interface{interface}
{
    // std::cout<< get_client_id.getID() << "\n";
    init();
}

void network::NetworkManager::connected()
{
    send(get_client_id, RPCTarget::Server, {});
}

void network::NetworkManager::init()
{
    network_manager = this;
    interface.setCallback([this](const std::string_view &data, uv_stream_t *client)
                          { receive(data, client); });
    interface.setCallback([this]()
                          { connected(); });
    std::thread([&]()
                { executeRPCs(); })
        .detach();
}

void network::NetworkManager::receive(const std::string_view &data, uv_stream_t *client)
{

    intptr_t client_ptr = reinterpret_cast<intptr_t>(client);

    ClientInfo &info = infos[client_ptr];
    info.append(data);
    info.client_stream = client;

    queue.push(&info);
    queue_wait.notify_one();
}

void network::NetworkManager::executeRPCs()
{
    while (true)
    {

        std::unique_lock<std::mutex> lock(queue_mutex);

        queue_wait.wait(lock, [this]
                        { return !queue.empty(); });

        auto *client = queue.front();
        queue.pop();

        while (true)
        {

            const auto message = client->createMessage();
            if (!message)
                break;

            RPCPacketHeader *header = (RPCPacketHeader *)((*message).data());

            const std::string_view view{(*message).data() + sizeof(RPCPacketHeader), (*message).size() - sizeof(RPCPacketHeader)};

            assert(rpcs.contains(header->rpc_id));

            const auto result = rpcs[header->rpc_id]->rpc(view);
            if (!result)
                continue;

            interface.send(*result, client->client_stream);
        }
    }
}

bool network::NetworkManager::isServer()
{
    return id == 0;
}

bool network::NetworkManager::isClient()
{
    return !isServer();
}
