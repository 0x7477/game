#pragma once
#include <string>
#include <functional>
#include <uv.h>
#include <iostream>
namespace network
{
    class NetworkInterface
    {
    public:
        NetworkInterface();

        virtual void send(const std::string_view &data);
        virtual void sendTo(const std::string_view &message, uv_stream_t *client);

        void setCallbackReceive(std::function<void(const std::string_view &, uv_stream_t *)> callback);
        void setCallbackConnected(std::function<void()> callback);
        void setCallbackNewConnection(std::function<void(uv_stream_t *)> callback);
        void setCallbackConnectionLost(std::function<void()> callback);

        std::function<void(const std::string_view &, uv_stream_t *)> receive;
        std::function<void()> connected;
        std::function<void(uv_stream_t *)> new_connection;
        std::function<void()> connection_lost;
    };
}