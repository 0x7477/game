#pragma once

#include <functional>
#include <iostream>
#include <map>
#include <cstring>
#include <source_location>
#include "datagram.hpp"
#include <optional>
#include <type_traits>
#include <any>
#include <vector>

namespace network
{
    inline std::map<unsigned int, std::vector<std::any>> synced_objects;

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
        constexpr RPCBase(){};
        virtual std::optional<SerializedDatagram> rpc(const std::string_view &data);
        // virtual rpc_id_t getID() const;
    };
    inline std::map<std::size_t, RPCBase *> rpcs;

    struct __attribute__((packed)) RPCPacketHeader
    {
        rpc_message_length_t message_length;
        rpc_id_t rpc_id;
        RPCTarget::RPCTarget target;
    };

    template <class input = Datagram<>, auto execution = [](const input &) -> std::optional<SerializedDatagram>
              { return {}; },
              typename response = void>
    class RPC : public RPCBase
    {
    public:
        constexpr RPC()
        {
            std::cout << this << " " << getID() << "\n";
            rpcs[getID()] = this;

            if constexpr (!std::is_same<response, void>::value)
            {
                response *res = new response();
                rpcs[response::getID()] = res;
            }
        }

        std::optional<SerializedDatagram> rpc(const std::string_view &data) override
        {
            using return_type = typename std::result_of<decltype(execution)(input)>::type;

            input dgram{SerializedDatagram{data}};
            if constexpr (std::is_same<return_type, void>::value)
            {
                execution(dgram);
                return {};
            }
            else
            {
                const auto serialized_response = execution(dgram);
                const SerializedDatagram serialized_datagram{response::serialize(RPCTarget::Client, serialized_response)};
                return {serialized_datagram};
            }
        };

        static constexpr rpc_id_t getID()
        {
            const std::string name{typeid(RPC<input, execution, response>).name()};
            const auto hash{std::hash<std::string>{}(name)};
            return hash;
        }

        operator std::string()
        {
            return serialize();
        }

        static std::string serialize(const RPCTarget::RPCTarget &target, const SerializedDatagram &data)
        {
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

        static std::string serialize(const RPCTarget::RPCTarget &target, const input &dgram)
        {
            return serialize(target, dgram.serialize());
        }
    };

    template <typename T>
    class Synced;

    template <std::size_t N, typename Tuple, typename Func, std::size_t... Is>
    void tupleToArgsHelper(const Tuple &t, Func &&f, std::index_sequence<Is...>)
    {
        f(std::get<N + Is>(t)...);
    }

    template <std::size_t N = 0, typename Tuple, typename Func>
    void tupleToArgsHelper(const Tuple &t, Func &&f)
    {
        constexpr std::size_t tuple_size = std::tuple_size_v<Tuple>;
        tupleToArgsHelper<N>(t, std::forward<Func>(f), std::make_index_sequence<tuple_size - N>{});
    }

    template <typename Member, auto function, typename... Args>
    class RPC<network::Datagram<unsigned int, unsigned int, Args...>,
              [](const network::Datagram<unsigned int, unsigned int, Args...> &data)
              {
                  auto &objects_list = network::synced_objects[std::get<0>(data.getData())];
                  auto &any = objects_list[std::get<1>(data.getData())];
                  network::Synced<Member> *member_pointer = std::any_cast<network::Synced<Member> *>(any);

                  tupleToArgs<2>(data.getData(), [&member_pointer](auto &&...args)
                              { member_pointer->t.function(std::forward<decltype(args)>(args)...); });
              }>
        MemberRPC;

}