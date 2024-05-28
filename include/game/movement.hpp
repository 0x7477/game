#pragma once
#include <array>

enum MovementType
{
    Foot, Mech, Treads, Tires, Ship, Lander, Air, Pipe, MovementTypeCount
};

struct MovementCosts
{
    MovementCosts(const std::array<unsigned,MovementTypeCount> & costs);
    
    unsigned getCosts(const MovementType& type);
    std::array<unsigned,MovementTypeCount> costs;
};
