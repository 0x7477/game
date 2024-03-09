#pragma once
#include "rpc.hpp"
#include <map>
#include <vector>
#include "network_manager.hpp"
#include <any>

namespace network
{

    template <typename T>
    class Synced;

    template <std::size_t N, typename Tuple, typename Func, std::size_t... Is>
    void tupleToArgsHelper(const Tuple &t, Func &&f, std::index_sequence<Is...>)
    {
        f(std::get<N + Is>(t)...);
    }

    template <std::size_t N, typename Tuple, typename Func>
    void tupleToArgsHelper(const Tuple &t, Func &&f)
    {
        constexpr std::size_t tuple_size = std::tuple_size_v<Tuple>;
        tupleToArgsHelper<N>(t, std::forward<Func>(f), std::make_index_sequence<tuple_size - N>{});
    }

    template <typename T, typename... Args>
    class RPC<network::Datagram<unsigned int, unsigned int, RPCTarget, Args...>, [](const auto &tuple)
              {
                  const auto &data = tuple.getData();
                  const unsigned int &player = std::get<0>(data);
                  const unsigned int &id{std::get<1>(data)};
                  const RPCTarget &target{std::get<2>(data)};

                  tupleToArgsHelper<3>(data, [&](auto &&...args)
                                       {
                  const T t{std::forward<decltype(args)>(args)...};
                  synced_objects[player].push_back(new Synced<T>{player, id, target, t}); });
              }>
        synced_constructor;

    template <typename T>
    class Synced
    {
    public:
        template <typename... Args>
        Synced(const RPCTarget &target, const Args &...arguments)
            : t{arguments...}, target{target}, owner{true}, player{NetworkManager::getID()}, id{(unsigned int)(synced_objects[player].size())}
        {
            synced_objects[player].push_back(this);
            std::cout << "created synced class (" << typeid(T).name() << ") owner: " << owner << "\n";

            const Datagram<unsigned int, unsigned int, RPCTarget, Args...> datagram{player, id, target, arguments...};

            NetworkManager::send(synced_constructor<T, Args...>, target & ~RPCTargets::Me, datagram);
        }
        template <typename... Args>
        Synced(const unsigned int &player, const unsigned int &id, const RPCTarget &target, Args... arguments)
            : t{arguments...}, target{target}, owner{false}, player{player}, id{id}
        {
            std::cout << "created synced class (" << typeid(T).name() << ") owner: " << owner << "\n";
        }

        template <class input = Datagram<>, auto execution = []() {}, typename response = void>
        void execute(RPC<input, execution, response> &rpc, auto... params)
        {
            network::NetworkManager::send(rpc, target, {player, id, params...});
        }

        // private:
        T t;
        RPCTarget target;
        bool owner{false};
        unsigned int player;
        unsigned int id;
    };
}
