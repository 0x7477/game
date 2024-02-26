#pragma once
#include <list>
#include "packet_list_element.hpp"
#include "rpc.hpp"
#include <ranges>
#include <uv.h>
namespace network
{
    struct ClientInfo
    {
        void append(const std::string_view &data);
        auto getView() const;
        void removeBytes(const std::size_t &number_of_bytes);
        std::optional<std::string> createMessage();
        std::optional<message_length_t> getMessageLength();

        int client_id{-1};
        uv_stream_t* client_stream;
        std::list<PacketListElement> packets{};
        std::size_t list_byte_size{0};
    };
}