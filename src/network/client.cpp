#include <network/client.hpp>

network::Client::Client(const std::string &ip, const unsigned int &port)
    : ip{ip}, port{port}, loop(uv_default_loop()), client(new uv_tcp_t)
{
    connect();
    std::thread([&]()
                { runEventLoop(); })
        .detach();
}

void network::Client::connect()
{
    uv_connect_t *connection_request = new uv_connect_t;

    connection_request->data = this;
    sockaddr_in destination_address;
    uv_ip4_addr(ip.c_str(), port, &destination_address);

    uv_tcp_init(loop, client);
    client->data = this;

    uv_tcp_connect(connection_request, client, reinterpret_cast<const sockaddr *>(&destination_address), [](uv_connect_t *req, int status)
                   {

            auto * client = static_cast<Client*>(req->data);
            if (status < 0) {
                std::cerr << "Connection error: " << uv_strerror(status) << std::endl;
                client->reconnect();
                client->connection_lost();
                uv_close(reinterpret_cast<uv_handle_t*>(req->handle), nullptr);
            } else {
                std::cout << "Connected to server" << std::endl;
                client->connected();
                client->startReceiving();
            }
            delete req; });
}


void network::Client::send(const std::string_view &message, uv_stream_t *client)
{
    send(message);
}
void network::Client::send(const std::string_view &message)
{
    uv_buf_t buf = uv_buf_init(const_cast<char *>(message.data()), message.size());
    uv_write_t *writeReq = new uv_write_t;

    uv_write(writeReq, reinterpret_cast<uv_stream_t *>(client), &buf, 1, [](uv_write_t *req, int status)
             {
            if (status < 0) {
                std::cerr << "Write error: " << uv_strerror(status) << std::endl;
            }
            
            delete req; });
}

void network::Client::startReceiving()
{
    uv_read_start(
        reinterpret_cast<uv_stream_t *>(client), [](uv_handle_t *handle, size_t suggested_size, uv_buf_t *buf)
        { *buf = uv_buf_init(new char[suggested_size], suggested_size); },
        [](uv_stream_t *stream, ssize_t nread, const uv_buf_t *buf)
        {
            if (nread > 0)
            {
                Client *client = (Client *)stream->data;
                client->receive(std::string_view{buf->base, (std::size_t)nread}, stream);
            }
            else if (nread < 0)
            {
                if (nread != UV_EOF)
                    std::cerr << "Read error: " << uv_strerror(nread) << std::endl;
                else
                    std::cerr << "Connection closed: " << uv_strerror(nread) << std::endl;

                uv_close(reinterpret_cast<uv_handle_t *>(stream), [](uv_handle_t *handle)
                         { delete[] handle; });
                auto *client = static_cast<Client *>(stream->data);
                client->reconnect();
            }
        });
}

void network::Client::runEventLoop()
{
    uv_run(loop, UV_RUN_DEFAULT);
}

void network::Client::reconnect()
{
    uv_timer_t *reconnectTimer = new uv_timer_t;
    reconnectTimer->data = this;
    uv_timer_init(loop, reconnectTimer);
    uv_timer_start(
        reconnectTimer, [](uv_timer_t *handle)
        {
            auto * client = static_cast<Client*>(handle->data);
            uv_timer_stop(handle);
            uv_close(reinterpret_cast<uv_handle_t*>(handle), nullptr);
            std::cout << "Reconnecting..." << std::endl;

            client->connect(); },
        reconnection_time, 0); // 3000 milliseconds delay
}
