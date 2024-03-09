#include <network/network_interface.hpp>

network::NetworkInterface::NetworkInterface(){};

void network::NetworkInterface::send(const std::string_view &data){};
void network::NetworkInterface::send(const std::string_view &message, uv_stream_t *client){};

void network::NetworkInterface::setCallbackReceive(std::function<void(const std::string_view &, uv_stream_t *)> callback)
{
    receive = callback;
}

void network::NetworkInterface::setCallbackConnected(std::function<void()> callback)
{
    connected = callback;
}

void network::NetworkInterface::setCallbackNewConnection(std::function<void(uv_stream_t *)> callback)
{
    new_connection = callback;
}

void network::NetworkInterface::setCallbackConnectionLost(std::function<void()> callback)
{
    connection_lost = callback;
}