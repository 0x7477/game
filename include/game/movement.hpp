#pragma once

enum MovementType
{
    Foot, Mech, Treads, Tires, Ship, Lander, Air, Pipe, MovementTypeCount
};

struct MovementCosts
{
    unsigned costs[MovementTypeCount];

    unsigned getCosts(const MovementType& type)
    {
        return costs[type];
    }
};
