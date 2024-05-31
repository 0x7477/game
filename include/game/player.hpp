#pragma once
#include <game/teams.hpp>
#include <string>

class Player
{
public:
    Player();
    Player(const std::string &name, const Team &team);

    std::string name{""};
    unsigned money{100000};
    Team team{Neutral};
};