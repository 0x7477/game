#pragma once

#include <uv.h>
#include <memory>
#include "rpc.hpp"
#include <queue>

#include "network_interface.hpp"
namespace network
{
    class Server : public NetworkInterface
    {
    public:
        Server(const std::string &ip, const unsigned int &port);
        void start();
        static void receivePacket(uv_stream_t *client, ssize_t nread, const uv_buf_t *buf);
        void onConnection(uv_stream_t *server);
        void runEventLoop();

        void sendTo(const std::string_view &message, uv_stream_t *client) override;

        static void sendMessageCallback(uv_async_t *handle);

        std::queue<std::tuple<std::string , uv_stream_t *>> send_queue{}; 
    private:
        std::map<uv_stream_t *, std::string> overhead_buffer;
        const std::string ip;
        const unsigned int port;
        uv_loop_t *loop;
        uv_async_t async;
        std::unique_ptr<uv_tcp_t> server;
    };
}