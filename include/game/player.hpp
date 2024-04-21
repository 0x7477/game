#pragma once
#include <string>
#include <iostream>
#include <engine/synced.hpp>
#include <engine/script.hpp>
#include <engine/game_object.hpp>
#include <engine/transform.hpp>
#include <chrono>

class Player : public Script
{
public:
    Player(GameObject *game_object, const std::string &name);
    Player() = default;

    // static void serialize(std::vector<char> &buffer, const Player &value);

    // static void deserialize(std::string_view &buffer, Player &value);

    void print()
    {
        std::cout << "name" <<"\n";
    }

    // void setHealth(const int &h)
    // {
    //     // health = h;
    // }

    void updatePosition(const Transform &transform);

    void syncPosition();

    virtual void update(const float& delta_time) override;
    void control(const double& delta_time);
    void interpolateReceivedMovement();
    virtual void display(sf::RenderWindow &window) override;

    // int health{100};

    // std::string name;

    Synced &synced;
};

inline auto print_rpc = network::MemberRPC<&Player::print>;
// inline auto set_health_rpc = network::MemberRPC<&Player::setHealth>;
// inline auto update_position_rpc = network::MemberRPC<&Player::updatePosition>;