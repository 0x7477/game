#pragma once

#include <uv.h>
#include <memory>
#include <iostream>
#include "rpc.hpp"
#include <thread>
#include <cstring>
#include "network_interface.hpp"

namespace network
{
    class Client : public NetworkInterface
    {
    public:
        Client(const std::string &ip, const unsigned int &port);

        void connect();

        void startReceiving();
        void runEventLoop();

        void send(const std::string_view &message) override;
        void sendTo(const std::string_view &message, uv_stream_t *) override;

    private:
        void reconnect();

        const std::string ip;
        const unsigned int port;

        static const unsigned int reconnection_time{1000};
        uv_loop_t *loop;
        uv_tcp_t *client;
    };
}