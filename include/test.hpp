#pragma once
#include "network/rpc.hpp"

class Player
{
public:
    Player(const std::string &name)
        : name{name}
    {
    }

    static network::RPC<network::Datagram<int>, [](const network::Datagram<int> &data)
                 {
                     std::cout << data.get<0>() << std::endl;// << "has " << data.get<0>() << "HP\n";
                 }>
        display_health;

    int health{100};
    const std::string name;
};