#include <network/network_manager.hpp>

auto say_hi(const network::Datagram<> &data)
{
    std::cout << "hi\n";
}

// inline auto say_hi_rpc = network::GlobalRPC<&say_hi>;

auto print_num(const network::Datagram<int> &data)
{
    std::cout << data.get<0>() << "\n";
    return network::Datagram<int>{data.get<0>()};
}
// inline auto print_num_rpc = network::GlobalRPC<&print_num>;

auto add(const network::Datagram<int, int> &data)
{
    std::cout << data.get<0>() << "+" << data.get<1>() << "\n";
    return network::Datagram<int>{data.get<0>() + data.get<1>()};
}

// inline auto add_rpc2 = network::GlobalRPC<&add>;
// inline auto a = network::GlobalRPC<&print_num>;
// inline auto b = network::GlobalRPC<&say_hi>;

inline auto add_rpc2 = network::RPCChain3<&print_num, &print_num,&print_num>;
// inline auto add_rpc2 = network::RPCChain3<&add, &print_num,&print_num>;
// inline auto add_then_hi_rpc = network::RPCChain2<&add, &print_num, &print_num,&print_num,&print_num>;
// inline auto add_rpc = network::GlobalRPC<&add, print_num_rpc>;


// network::RPC<network::Datagram<>, [](const auto &data)
//              {
//                  static unsigned int max_id = 1;
//                  return network::Datagram{max_id++};
//              },
//              network::RPC<network::Datagram<unsigned int>, [](const auto &data)
//                           {
//                 network::network_manager->setID(data.get());
//                 std::cout << "set ID to "<< data.get() << "\n"; }>>
//     get_client_id;

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
    // send(get_client_id, RPCTarget::Server, {});
    // send(add_rpc, RPCTarget::Server, {1, 2});
    // send(add_then_hi_rpc, RPCTarget::Server, {1,2});
    send(add_rpc2, RPCTarget::Server, {1});
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

            std::cout << "execute RPC\n";

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
