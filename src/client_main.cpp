

#include <iostream>
#include <network/client.hpp>
#include <network/rpcs/echo.hpp>
#include <network/rpcs/add_one.hpp>
#include <network/network_manager.hpp>

#include "test.hpp"

int main(int argc, char **argv)
{
    network::Client client{"127.0.0.1", 2000};

    network::NetworkManager manager{client};

    sleep(1);


    // manager.send(echo,network::RPCTarget::Server, {"abc"});
    // manager.send(echo,network::RPCTarget::Server, {"def"});
    // manager.send(echo,network::RPCTarget::Server, {"hij"});

    // std::vector<int> data{};
    // for (int i = (1 << 15) - 1; i-- > 0;)
    //     data.push_back(data.size());

    // while (true)
    // {
    //     client.send(vector.serialize(network::RPCTarget::Server, {data}));

    //     // for(int i = 1000; i --> 0;)

    //     // data.push_back(data.size());
    //     // sleep(1);

    //     // client.sendRPC(add_one, network::RPCTarget::Server);
    //     // client.sendMessage(add_one.serialize(network::RPCTarget::Server, {}));
    //     // std::cout << __FILE__ << ":"<< __LINE__ << std::endl;
    //     // network::Datagram<int, std::string> d{4, "Kurt"};
    //     // const std::string serialized{d};
    //     // std::cout << __FILE__ << ":"<< __LINE__ << std::endl;

    //     // std::cout << serialized << std::endl;
    //     // client.sendRPC(Player::display_health, network::RPCTarget::Server, serialized);
    //     std::cout << "..." << std::endl;
    // }
}