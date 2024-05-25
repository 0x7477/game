#include <game/unit.hpp>

std::map<std::string, unsigned> Unit::unit_costs{
    {INFANTRY, 1000},
    {TCOPTER, 5000},
    {ARTILLERY, 1000},
    {TANK, 7000},
};
