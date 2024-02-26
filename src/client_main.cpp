

#include <iostream>
#include <network/client.hpp>
#include <network/rpcs/echo.hpp>
#include <network/rpcs/add_one.hpp>
#include <network/network_manager.hpp>

#include "player.hpp"
#include "game.hpp"
#include <network/synced.hpp>

int main(int argc, char **argv)
{

    // network::Datagram<Player> players{Player{"Peter"}};
    // Player player{Player{"template class network::Synced<>;"}};

    // std::vector<char> buffer{};

    // network::serialize_element(buffer, player);
    // std::cout << buffer.size() << "\n";

    // Player player2{""};
    // player2.health = 2;

    // std::string_view view{buffer.data(), buffer.size()};
    // network::deserialize_element(view, player2);

    // std::cout << player2.name << "\n";
    // std::cout << player2.health << "\n";

    network::Client client{"127.0.0.1", 2000};

    network::NetworkManager manager{client};

    sleep(1);

    Game game{};
    // std::cout <<std::is_trivially_copyable<Player>::value << "\n";
    // network::Synced<int> player{{2}};

    // player.t++;

    // network::Synced<int> player{{2}};

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