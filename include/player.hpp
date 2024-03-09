#pragma once
#include <string>
#include <iostream>
#include <network/synced.hpp>
#include "game_object.hpp"
class Player: public GameObject
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


    virtual void update(sf::RenderWindow& window) override;
    virtual void display(sf::RenderWindow& window) override;

    int health{100};

    float pos_x{0}, pos_y{0};
    std::string name;
};


inline auto print_rpc = network::MemberRPC<&Player::print>;
inline auto set_health_rpc = network::MemberRPC<&Player::setHealth>;

template class network::Synced<Player>;
