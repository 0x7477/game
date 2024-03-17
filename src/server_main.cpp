#include <network/server.hpp>
#include <network/datagram.hpp>

#include <network/network_manager.hpp>
#include <scenes/game.hpp>

int main(int argc, char **argv)
{
    network::Server server{"0.0.0.0", 2000};
    network::NetworkManager manager{server};

    server.start();

    server.runEventLoop();

    return 0;
}