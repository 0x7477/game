

#include <iostream>
#include <network/client.hpp>
#include <network/network_manager.hpp>

#include "player.hpp"
#include "game.hpp"
#include <network/synced.hpp>
#include <thread>

int main(int argc, char **argv)
{
    {
    network::Client client{"127.0.0.1", 2000};

    {

        network::NetworkManager manager{client};

        {

            Game game{manager};

            game.run();
            std::cout << __FILE__ << ":" << __LINE__ << std::endl;
        }

        std::cout << __FILE__ << ":" << __LINE__ << std::endl;
    }
    std::cout << __FILE__ << ":" << __LINE__ << std::endl;
    }
    std::cout << __FILE__ << ":" << __LINE__ << std::endl;
}