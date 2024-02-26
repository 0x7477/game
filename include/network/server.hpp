#pragma once

#include <uv.h>
#include <memory>
#include "rpc.hpp"

#include "network_interface.hpp"
namespace network
{
    class Server : public NetworkInterface
    {
    public:
        Server(const std::string &ip, const unsigned int &port);
        void start();
        static void processPacket(const std::string_view &data, const RPCPacketHeader *header);
        static void receivePacket(uv_stream_t *client, ssize_t nread, const uv_buf_t *buf);
        void onConnection(uv_stream_t *server);
        void send(const std::string_view &message, uv_stream_t *client) override;
        void runEventLoop();

    private:
        std::map<uv_stream_t *, std::string> overhead_buffer;
        const std::string ip;
        const unsigned int port;
        uv_loop_t *loop;
        std::unique_ptr<uv_tcp_t> server;
    };
}