#pragma once

#include <functional>
#include <iostream>
#include <map>
#include <cstring>
#include <source_location>
#include "datagram.hpp"
namespace network
{
    typedef uint32_t rpc_message_length_t;
    typedef uint32_t rpc_id_t;
    namespace RPCTarget
    {
        enum RPCTarget : uint8_t
        {
            Server,
            All,
            Client
        };
    }

    class RPCBase
    {
    public:
        RPCBase() {}
        virtual void rpc(const std::string_view &data){};
        virtual rpc_id_t getID(const std::source_location location = std::source_location::current()) const { return 0; };
    };
    inline std::map<std::size_t, RPCBase *> rpcs;

    struct __attribute__((packed)) RPCPacketHeader
    {
        rpc_message_length_t message_length;
        rpc_id_t rpc_id;
        RPCTarget::RPCTarget target;
    };
    template <class input = Datagram<>, auto execution = [](const input &) {}, typename output = void >
    class RPC : public RPCBase
    {
    public:
        RPC()
        {
            std::cout << getID() <<"\n";
            rpcs[getID()] = this;
        }
        output rpc(const std::string_view &data) override
        {
            input dgram{SerializedDatagram{data}};
            return execution(dgram);
        };

        constexpr rpc_id_t getID(const std::source_location location = std::source_location::current()) const override
        {
            const std::string name{location.function_name()};

            std::cout << name << "\n";
            const auto hash{std::hash<std::string>{}(name )};
            return hash;
        }

        operator std::string()
        {
            return serialize();
        }

        std::string serialize(const RPCTarget::RPCTarget &target, const input &dgram) const
        {
            const std::string data{dgram.serialize()};

            std::vector<char> message;
            const rpc_message_length_t message_size{(rpc_message_length_t)(sizeof(RPCPacketHeader) + data.size())};
            message.reserve(message_size);

            RPCPacketHeader *header = (RPCPacketHeader *)message.data();

            header->message_length = message_size;
            header->rpc_id = getID();
            header->target = target;

            std::memcpy(header + 1, data.data(), data.size());
            return std::string(message.data(), message_size);
        }
    };

}