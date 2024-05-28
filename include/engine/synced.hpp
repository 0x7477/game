#pragma once
#include <network/rpc.hpp>
#include <map>
#include <vector>
#include <network/network_manager.hpp>
#include <any>
#include <engine/game_object.hpp>
#include <engine/component.hpp>

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

void construct(const network::Datagram<unsigned int, unsigned int, network::RPCTarget> &tuple);

network::RPC<network::Datagram<unsigned int, unsigned int, network::RPCTarget>, [](const auto &tuple)
                   { construct(tuple); }>
    synced_constructor{"synced_constructor"};

template <typename T, typename... Args>
class network::RPC<network::Datagram<unsigned int, unsigned int, Args...>, [](const auto &tuple)
                   {
                       const auto &data = tuple.getData();
                       const unsigned int &player = std::get<0>(data);
                       const unsigned int &id{std::get<1>(data)};

                       tupleToArgsHelper<2>(data, [&](auto &&...args)
                                            {
        const auto game_object = network::synced_objects[player][id];
        game_object->addComponent<T>(std::forward<decltype(args)>(args)...); });
                   }>
    add_component{"add component"};

class Synced : public Component
{
public:
    Synced(GameObject *game_object, const network::RPCTarget &target);
    Synced(GameObject *game_object, const unsigned int &player, const unsigned int &id, const network::RPCTarget &target);

    template <typename Component, typename... Args>
    void addComponent(const Args &...args)
    {
        network::NetworkManager::send(add_component<Component, Args...>, target, network::Datagram<unsigned int, unsigned int, Args...>{player, id, args...});
    }

    template <typename Component, typename... Args>
    void addComponentAt(const network::RPCTarget &t, const Args &...args)
    {
        network::NetworkManager::send(add_component<Component, Args...>, t, network::Datagram<unsigned int, unsigned int, Args...>{player, id, args...});
    }

    template <class input = network::Datagram<>, auto execution = []() {}, typename response = void>
    void executeAt(network::RPC<input, execution, response> &rpc,const network::RPCTarget &t, auto... params)
    {
        network::NetworkManager::send(rpc, t, {player, id, params...});
    }
    template <class input = network::Datagram<>, auto execution = []() {}, typename response = void>
    void execute(network::RPC<input, execution, response> &rpc, auto... params)
    {
        network::NetworkManager::send(rpc, target, {player, id, params...});
    }

    bool isOwner() const
    {
        return owner;
    }

private:
    network::RPCTarget target;
    bool owner{false};
    unsigned int player;
    unsigned int id;
};
