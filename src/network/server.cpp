#include <network/server.hpp>

network::Server::Server(const std::string &ip, const unsigned int &port)
    : ip{ip}, port{port}, loop(uv_default_loop()), server(new uv_tcp_t)
{
    uv_tcp_init(loop, server.get());
    server->data = this;
}

void network::Server::start()
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

void network::Server::receivePacket(uv_stream_t *client, ssize_t nread, const uv_buf_t *buf)
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
}

void network::Server::onConnection(uv_stream_t *server)
{
    uv_tcp_t *client = new uv_tcp_t;

    client->data = this;
    uv_tcp_init(loop, client);

    if (uv_accept(server, reinterpret_cast<uv_stream_t *>(client)) == 0)
    {
        uv_read_start(
            reinterpret_cast<uv_stream_t *>(client), [](uv_handle_t *, size_t suggested_size, uv_buf_t *buf)
            {
                buf->base = new char[suggested_size];
                buf->len = suggested_size; },
            receivePacket);

        Server *server = (Server *)client->data;
        server->new_connection(reinterpret_cast<uv_stream_t *>(client));
    }
    else
    {
        uv_close(reinterpret_cast<uv_handle_t *>(client), [](uv_handle_t *handle)
                 { 
                            std::cout << "delete handle\n";
                            delete[] handle; 
                });
    }
}

void network::Server::sendTo(const std::string_view &message, uv_stream_t *client)
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

void network::Server::runEventLoop()
{
    uv_run(loop, UV_RUN_DEFAULT);
}
