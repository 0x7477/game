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

    template <class input = Datagram<>, auto execution = []() {}, typename response = void>
    class RPC : public RPCBase
    {
    public:
        constexpr RPC()
        {
            // std::cout << this << " " << getID() << "\n";
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
            }
            else if constexpr (!std::is_same<response, void>::value)
            {
                const auto serialized_response = execution(dgram);
                const SerializedDatagram serialized_datagram{response::serialize(RPCTarget::Client, serialized_response)};
                return {serialized_datagram};
            }

            return {};
        };

        static constexpr rpc_id_t getID()
        {

            std::cout << __PRETTY_FUNCTION__ << std::endl << "==============\n";
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

    template <typename Member, std::size_t... Is, typename Tuple, typename Func>
    constexpr inline void call_member(const Tuple &t, Func &&f, std::index_sequence<Is...>, network::Synced<Member> *member_pointer)
    {
        std::invoke(f, &member_pointer->t, std::get<2 + Is>(t)...);
    }

    template <class Sig>
    struct member_function_datagram;
    template <class R, class C, class... Args>
    struct member_function_datagram<R (C::*)(Args...)>
    {
        using datagram = Datagram<unsigned int, unsigned int, typename std::remove_reference<Args>::type...>;
        using member_class = C;
        constexpr static std::size_t size = sizeof...(Args);
    };

    template <class R, class... Args>
    struct member_function_datagram<R(Args...)>
    {
        using datagram = Datagram<unsigned int, unsigned int, typename std::remove_reference<Args>::type...>;
        constexpr static std::size_t size = sizeof...(Args);
    };

    template <auto function>
    inline class RPC<typename member_function_datagram<decltype(function)>::datagram,
                     [](const auto &data)
                     {
                         using member = typename member_function_datagram<decltype(function)>::member_class;
                         const auto &objects_list = network::synced_objects[std::get<0>(data.getData())];
                         const auto &any = objects_list[std::get<1>(data.getData())];
                         network::Synced<member> *member_pointer = std::any_cast<network::Synced<member> *>(any);

                         call_member(data.getData(), function, std::make_index_sequence<member_function_datagram<decltype(function)>::size>{}, member_pointer);
                     }>
        MemberRPC;

    template <class C>
    struct global_function_datagram;

    template <class R, class... Args>
    struct global_function_datagram<R(Datagram<Args...>)>
    {
        using datagram = Datagram<typename std::remove_reference<Args>::type...>;
        constexpr static std::size_t size = sizeof...(Args);
    };

    template <class R, class... Args>
    struct global_function_datagram<R (*)(const Datagram<Args...> &)>
    {
        using datagram = Datagram<typename std::remove_reference<Args>::type...>;
        constexpr static std::size_t size = sizeof...(Args);
    };

    // template <auto call>
    // inline class RPC<typename global_function_datagram<decltype(call)>::datagram, call> GlobalRPC;

    template <auto call>
    inline class RPC<typename global_function_datagram<decltype(call)>::datagram, call> GlobalRPC;

    template <auto call, auto response>
    inline class RPC<typename global_function_datagram<decltype(call)>::datagram, call, RPC<typename global_function_datagram<decltype(response)>::datagram, response>> RPCChain2;

    template <auto call, auto response1, auto response2>
    inline class RPC<typename global_function_datagram<decltype(call)>::datagram, call, RPC<typename global_function_datagram<decltype(response1)>::datagram, response1, RPC<typename global_function_datagram<decltype(response2)>::datagram, response2>>> RPCChain3;

}