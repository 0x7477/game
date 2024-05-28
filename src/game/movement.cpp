#include <game/movement.hpp>

MovementCosts::MovementCosts(const std::array<unsigned, MovementTypeCount> &costs)
    : costs{costs}
{
}

unsigned MovementCosts::getCosts(const MovementType &type)
{
    return costs[type];
}