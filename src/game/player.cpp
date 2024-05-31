#include <game/player.hpp>
#include <iostream>
Player::Player() {}
    Player::Player(const std::string &name, const Team &team)
        : name{name}, team{team} {}