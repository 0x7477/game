#include <network/server.hpp>
#include <network/datagram.hpp>

#include <network/rpcs/echo.hpp>
#include <network/rpcs/add_one.hpp>
#include <network/network_manager.hpp>
#include <network/synced.hpp>

#include "game.hpp"

int main(int argc, char **argv)
{
    network::Server server{"0.0.0.0", 2000};
    network::NetworkManager manager{server};

    server.start();

    server.runEventLoop();

    return 0;
}