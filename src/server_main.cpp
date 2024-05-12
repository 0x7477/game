#include <network/server.hpp>
#include <network/datagram.hpp>

#include <network/network_manager.hpp>

int main(int, char **)
{
    network::Server server{"0.0.0.0", 2001};
    network::NetworkManager manager{server};

    server.start();

    server.runEventLoop();

    return 0;
}