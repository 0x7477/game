#pragma once

#include <uv.h>
#include <memory>
#include <iostream>
#include <thread>
#include "rpc.hpp"
#include "rpcs/echo.hpp"
#include <optional>

#include "network_interface.hpp"
namespace network
{
    class Server : public NetworkInterface
    {
    public:
        Server(const std::string &ip, const unsigned int &port)
            : ip{ip}, port{port}, loop(uv_default_loop()), server(new uv_tcp_t)
        {
            uv_tcp_init(loop, server.get());
            server->data = this;
        }

        void start()
        {
            struct sockaddr_in addr;
            uv_ip4_addr(ip.c_str(), port, &addr);

            uv_tcp_bind(server.get(), reinterpret_cast<const struct sockaddr *>(&addr), 0);
            int r = uv_listen(reinterpret_cast<uv_stream_t *>(server.get()), SOMAXCONN, [](uv_stream_t *server, int status)
                              {
                if (status < 0) {
                    std::cerr << "Error on listen: " << uv_strerror(status) << std::endl;
                    return;
                }

                Server* srv = static_cast<Server*>(server->data);
                srv->onConnection(server); });

            if (r)
            {
                std::cerr << "Error on listen: " << uv_strerror(r) << std::endl;
                return;
            }

            std::cout << "Server listening on " << ip << ":" << port << std::endl;
        }

        static void processPacket(const std::string_view &data, const RPCPacketHeader *header)
        {
            std::cout << "processPacket data.size() " << data.size() << "\n";
            if (!rpcs.contains(header->rpc_id))
                std::cout << "header->rpc_id " << header->rpc_id << "\n";

            // int i =0;
            // for(const auto& c : data)
            // std::cout<< i << ": " << (int) c << "\n";
            rpcs[header->rpc_id]->rpc({data.data() + sizeof(RPCPacketHeader), data.size() - sizeof(RPCPacketHeader)});
        }

        static void receivePacket(uv_stream_t *client, ssize_t nread, const uv_buf_t *buf)
        {
            if (nread < 0)
            {
                if (nread != UV_EOF)
                    std::cerr << "Read error: " << uv_strerror(nread) << std::endl;

                uv_close(reinterpret_cast<uv_handle_t *>(client), [](uv_handle_t *handle)
                         { delete[] handle; });
                return;
            }

            Server *server = (Server *)client->data;
            server->receive(std::string_view{buf->base, (std::size_t)nread}, client);

            // ssize_t packet_offset{0};
            // auto &buffer = server.overhead_buffer[client];

            // if (buffer.size() > 0)
            // {
            //     std::cout<< "buffer.size() " << buffer.size() << "\n";
            //     // we have data from the previous message
            //     if (buffer.size() < sizeof(RPCPacketHeader::message_length))
            //     {
            //         // we still dont have the full size of the header
            //         const std::size_t missing_header_size_bytes = sizeof(RPCPacketHeader::message_length) - buffer.size();
            //         buffer += std::string{buf->base, missing_header_size_bytes};
            //         nread -= missing_header_size_bytes;
            //         packet_offset += missing_header_size_bytes;
            //     }
            //     RPCPacketHeader *header = (RPCPacketHeader *)buffer.data();
            //     const ssize_t remaining_bytes{(ssize_t)(header->message_length - buffer.length())};

            //     std::cout<< "header->message_length " << header->message_length << "\n";
            //     std::cout<< "remaining_bytes " << remaining_bytes << "\n";
            //     if (nread >= remaining_bytes)
            //     {
            //         std::cout<< "increase buffer" << "\n";
            //         buffer += std::string{buf->base + packet_offset, (std::size_t)remaining_bytes};

            //         std::cout << __LINE__ << " server.processPacket" << "\n";

            //         server.processPacket(buffer, header);

            //         packet_offset += remaining_bytes;
            //         nread -= remaining_bytes;
            //         buffer.clear();
            //     }
            //     else
            //     {
            //         std::cout<< "message still doesn't fit" << "\n";
            //         // our message still doesn't fit

            //         std::cout << "buffer.size()" << buffer.size() << "\n";
            //         buffer += std::string{buf->base + packet_offset, (std::size_t)nread};
            //         std::cout << "buffer.size()" << buffer.size() << "\n";

            //         packet_offset += nread;
            //         nread -= nread;
            //     }
            // }

            // std::cout << "nread " << nread  << "\n";
            // while (nread > 0)
            // {
            //     RPCPacketHeader *header = (RPCPacketHeader *)(buf->base + packet_offset);

            //     if ((std::size_t)nread >= sizeof(header->message_length) && nread >= header->message_length)
            //     {
            //         const std::string_view message{buf->base + packet_offset, header->message_length};
            //         std::cout << __LINE__ << " server.processPacket" << "\n";
            //         processPacket(message, header);

            //         packet_offset += header->message_length;
            //         nread -= header->message_length;
            //     }
            //     else // store message for later use
            //     {
            //         // if ((std::size_t)nread >= sizeof(header->message_length))
            //         //     buffer.reserve(std::max(std::size_t(0), buffer.capacity()- header->message_length));

            //         std::cout<< "added  " << nread << " remaining bytes to buffer" << "\n";

            //         buffer += std::string{buf->base + packet_offset, (std::size_t)nread};

            //         std::cout << "packet length is: "<< *(unsigned int*) buffer.data() << "\n";

            //         nread = 0;
            //     }
            // }
            // std::cout << "done\n";
        }

        void onConnection(uv_stream_t *server)
        {
            uv_tcp_t *client = new uv_tcp_t;

            client->data = this;
            uv_tcp_init(loop, client);

            if (uv_accept(server, reinterpret_cast<uv_stream_t *>(client)) == 0)
            {
                uv_read_start(
                    reinterpret_cast<uv_stream_t *>(client), [](uv_handle_t *handle, size_t suggested_size, uv_buf_t *buf)
                    {
                buf->base = new char[suggested_size];
                buf->len = suggested_size; },
                    receivePacket);
            }
            else
            {
                uv_close(reinterpret_cast<uv_handle_t *>(client), [](uv_handle_t *handle)
                         { 
                            std::cout << "delete handle\n";
                            delete[] handle; });
            }
        }

        void send(const std::string_view &message, uv_stream_t *client) override
        {
            uv_buf_t buf = uv_buf_init(const_cast<char *>(message.data()), message.size());
            uv_write_t *writeReq = new uv_write_t;
            uv_write(writeReq, client, &buf, 1, [](uv_write_t *req, int status)
                     {
            if (status < 0) {
                std::cerr << "Write error: " << uv_strerror(status) << std::endl;
            }
            delete req; });
        }

        void runEventLoop()
        {
            uv_run(loop, UV_RUN_DEFAULT);
        }

    private:
        std::map<uv_stream_t *, std::string> overhead_buffer;
        const std::string ip;
        const unsigned int port;
        uv_loop_t *loop;
        std::unique_ptr<uv_tcp_t> server;
    };
}