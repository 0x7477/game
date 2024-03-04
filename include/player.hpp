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

    void setHealth(const int& h)
    {
        health = h;
    }

    int health{100};
    std::string name;
};


// inline auto print_rpc = network::MemberRPC<&Player::print>;
// inline auto set_health_rpc = network::MemberRPC<&Player::setHealth>;

// template class network::Synced<Player>;
