#pragma once
#include <string>
#include <functional>
#include <uv.h>

namespace network
{
    class NetworkInterface
    {
    public:
        NetworkInterface();

        virtual void send(const std::string_view &data);
        virtual void send(const std::string_view &message, uv_stream_t *client);

        void setCallback(std::function<void(const std::string_view &, uv_stream_t *)> callback);

        void setCallback(std::function<void()> callback);

        std::function<void(const std::string_view &, uv_stream_t *)> receive;
        std::function<void()> connected;
    };
}