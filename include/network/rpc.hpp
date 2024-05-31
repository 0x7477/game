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
#include <engine/game_object.hpp>


namespace network
{
    inline std::map<unsigned int, std::vector<GameObject*>> synced_objects;

    typedef uint32_t rpc_message_length_t;
    typedef uint32_t rpc_id_t;
    typedef uint8_t RPCTarget;
    inline std::map<rpc_id_t,rpc_id_t> id_translator{};

    namespace RPCTargets
    {
        const unsigned int Me{1 << 0};
        const unsigned int Server{1 << 1};
        const unsigned int Clients{1 << 2};
        const unsigned int Buffered{1 << 3};

        const unsigned int AllClients = Me | Clients; 
        const unsigned int AllClientsBuffered = AllClients | Buffered; 
        const unsigned int ServerBuffered = Server | Buffered; 
        const unsigned int All = AllClients | Server; 
        const unsigned int AllBuffered = All | Buffered; 
    }

    class RPCBase
    {
    public:
        constexpr RPCBase(){};
        virtual std::optional<SerializedDatagram> rpc(const std::string_view &data);
    };
    inline std::map<std::size_t, RPCBase *> rpcs;

    struct __attribute__((packed)) RPCPacketHeader
    {
        rpc_message_length_t message_length;
        rpc_id_t rpc_id;
        RPCTarget target;
    };

    template <class input = Datagram<>, auto execution = []() {}, typename response = void>
    class RPC : public RPCBase
    {
    public:
        constexpr RPC(const std::string& id)
        {
            id_translator[getID()] = hash(id);
            rpcs[hash(id)] = this;

            std::cout << "rpc: " << id << " " << hash(id) << std::endl;
            if constexpr (!std::is_same<response, void>::value)
                new response(id+"_");
        }

        std::optional<SerializedDatagram> rpc(const std::string_view &data) override
        {
            using return_type = typename std::result_of<decltype(execution)(input)>::type;

            input dgram{SerializedDatagram{data}};

            if constexpr (!std::is_same<response, void>::value && !std::is_same<return_type, void>::value)
            {
                const auto serialized_response = execution(dgram);
                const SerializedDatagram serialized_datagram{response::serialize(RPCTargets::Clients | RPCTargets::Server, serialized_response)};
                return {serialized_datagram};
            }

            execution(dgram);

            return {};
        };

        void local_rpc(const input &data)
        {
            execution(data);
        };

        static constexpr rpc_id_t getID()
        {
            const std::string name{typeid(RPC<input, execution, response>).name()};
            const auto hash{std::hash<std::string>{}(name)};
            return hash;
        }

        static constexpr rpc_id_t hash(const std::string& string)
        {
            const auto hash{std::hash<std::string>{}(string)};
            return hash;
        }


        operator std::string()
        {
            return serialize();
        }

        static std::string serialize(const RPCTarget &target, const SerializedDatagram &data)
        {
            std::vector<char> message;
            const rpc_message_length_t message_size{(rpc_message_length_t)(sizeof(RPCPacketHeader) + data.size())};
            message.reserve(message_size);

            RPCPacketHeader *header = (RPCPacketHeader *)message.data();

            header->message_length = message_size;
            header->rpc_id = id_translator[getID()];
            header->target = target;

            std::memcpy(header + 1, data.data(), data.size());
            return std::string(message.data(), message_size);
        }

        static std::string serialize(const RPCTarget &target, const input &dgram)
        {
            return serialize(target, dgram.serialize());
        }
    };

    class Synced;

    template <typename Member, std::size_t... Is, typename Tuple, typename Func>
    constexpr inline void call_member(const Tuple &t, Func &&f, std::index_sequence<Is...>, Member *member_pointer)
    {
        std::invoke(f, member_pointer, std::get<2 + Is>(t)...);
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
                         GameObject* game_object = objects_list[std::get<1>(data.getData())];
                         member* component = &game_object->getComponent<member>();

                        // std::cout << typeid(member).name() << "\n";
                         call_member(data.getData(), function, std::make_index_sequence<member_function_datagram<decltype(function)>::size>{}, component);
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

    // template <auto call, auto response>
    // inline class RPC<typename global_function_datagram<decltype(call)>::datagram, call,response> GlobalRPCWithResponse;

    template <auto call, auto... responses>
    class RPCChainN;

    template <auto call, auto response, auto... responses>
    class RPCChainN<call, response, responses...>
    {
    public:
        using current_datagram = typename global_function_datagram<decltype(call)>::datagram;
        using next_chain = RPCChainN<response, responses...>;

        using type = RPC<current_datagram, call, typename next_chain::type>;
    };

    // Spezialfall für die Endbedingung
    template <auto call>
    class RPCChainN<call>
    {
    public:
        using current_datagram = typename global_function_datagram<decltype(call)>::datagram;
        using type = RPC<current_datagram, call>;
    };

    // Alias für bequeme Verwendung
    template <auto call, auto... responses>
    using RPCChain = typename RPCChainN<call, responses...>::type;
}