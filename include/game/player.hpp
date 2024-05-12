#pragma once
#include <game/teams.hpp>
#include <string>

class Player
{
public:
    Player() {}
    Player(const std::string &name, const Team &team)
        : name{name}, team{team}
    {
    }

    std::string name{""};
    unsigned money{0};
    Team team{Neutral};
};