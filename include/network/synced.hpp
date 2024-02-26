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

    template <typename T>
    class RPC<network::Datagram<unsigned int, unsigned int, T>, [](const network::Datagram<unsigned int, unsigned int, T> &data)
                              {
                                  const unsigned int &player = std::get<0>(data.getData());
                                  const unsigned int &id{std::get<1>(data.getData())};
                                  const T &t{std::get<2>(data.getData())};
                                  synced_objects[player].push_back(new Synced<T> {player, id, t});
                              }>
        synced_constructor;

    template <typename T>
    class Synced
    {
    public:
        Synced(const T &t)
            : t{t}, owner{true}, player{network_manager->getID()}, id{(unsigned int)(synced_objects[player].size())}
        {
            synced_objects[player].push_back(this);
            std::cout << "created synced class. owner: " << owner << "\n";

            const Datagram<unsigned int,unsigned int,T> datagram{player, id, t};

            network_manager->send(synced_constructor<T>, RPCTarget::All, datagram);
        }
        Synced(const unsigned int &player, const unsigned int &id, const T &t)
            : t{t}, owner{false}, player{player}, id{id}
        {
            std::cout << "created synced class. owner: " << owner << "\n";
        }


        // private:
        T t;
        bool owner{false};
        unsigned int player;
        unsigned int id;
    };
}
