#include <player.hpp>
#include <network/synced.hpp>

Player::Player(const std::string &name)
    : name{name} {}

void Player::serialize(std::vector<char> &buffer, const Player &value)
{
    network::serialize_element(buffer, value.health);
    network::serialize_element(buffer, value.name);
}

void Player::deserialize(std::string_view &buffer, Player &value)
{
    network::deserialize_element(buffer, value.health);
    network::deserialize_element(buffer, value.name);
}

