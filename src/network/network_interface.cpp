#include <network/network_interface.hpp>

network::NetworkInterface::NetworkInterface(){};

void network::NetworkInterface::send(const std::string_view &data){};
void network::NetworkInterface::send(const std::string_view &message, uv_stream_t *client){};

void network::NetworkInterface::setCallback(std::function<void(const std::string_view &, uv_stream_t *)> callback)
{
    receive = callback;
}

void network::NetworkInterface::setCallback(std::function<void()> callback)
{
    connected = callback;
}
