#include <game/player.hpp>

Player::Player() {}
    Player::Player(const std::string &name, const Team &team)
        : name{name}, team{team}
    {
    }
