#pragma once
#include <string>
#include <iostream>
#include <network/synced.hpp>

class Player
{
public:
    Player(const std::string &name);
    Player() = default;

    static void serialize(std::vector<char> &buffer, const Player &value);

    static void deserialize(std::string_view &buffer, Player &value);

    void print()
    {
        std::cout << name << " (" << health << ")\n";
    }

    int health{100};
    std::string name;
};

// inline network::MemberRPC<Player,print> print_rpc;

inline network::RPC<network::Datagram<unsigned int, unsigned int>,
                    [](const network::Datagram<unsigned int, unsigned int> &data)
                    {
                        auto &objects_list = network::synced_objects[data.get<0>()];
                        auto &any = objects_list[data.get<1>()];
                        network::Synced<Player> *player_pointer = std::any_cast<network::Synced<Player> *>(any);
                        player_pointer->t.print();
                    }>
    print_rpc;

template class network::Synced<Player>;
