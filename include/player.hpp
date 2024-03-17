#pragma once
#include <string>
#include <iostream>
#include <synced.hpp>
#include <script.hpp>
#include "game_object.hpp"
#include "transform.hpp"
#include <chrono>

class Player : public Script
{
public:
    Player(GameObject *game_object, const std::string &name);
    Player() = default;

    static void serialize(std::vector<char> &buffer, const Player &value);

    static void deserialize(std::string_view &buffer, Player &value);

    void print()
    {
        std::cout << name << " (" << health << ")\n";
    }

    void setHealth(const int &h)
    {
        health = h;
    }

    void updatePosition(const Transform &transform);

    void syncPosition();

    virtual void update(const double& delta_time) override;
    void control(const double& delta_time);
    void interpolateReceivedMovement();
    virtual void display(sf::RenderWindow &window) override;

    int health{100};

    Transform transform{};
    std::string name;

    Synced &synced;


    std::array<std::tuple<Transform, std::chrono::time_point<std::chrono::system_clock>>, 2> received_messages;
    std::size_t received_messages_count{0};
    Transform last_transform_on_message_arrival;
    
    sf::Texture hexagon;
};

inline auto print_rpc = network::MemberRPC<&Player::print>;
inline auto set_health_rpc = network::MemberRPC<&Player::setHealth>;
inline auto update_position_rpc = network::MemberRPC<&Player::updatePosition>;