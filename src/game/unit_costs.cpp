#include <game/unit.hpp>

std::map<std::string, unsigned> Unit::unit_costs{
    {MISSILE, 8000},
    {APC, 5000},
    {ARTILLERY, 6000},
    {BATTLECOPTER, 9000},
    {BATTLESHIP, 28000},
    {BLACKBOAT, 7500},
    {BOMBER, 22000},
    {CARRIER, 30000},
    {CRUISER, 18000},
    {FIGHTER, 20000},
    {INFANTRY, 1000},
    {LANDER, 12000},
    {MEDIUMTANK, 16000},
    {MECH, 3000},
    {MEGATANK, 28000},
    {MISSILE, 12000},
    {NEOTANK, 22000},
    {PIPERUNNER, 20000},
    {RECON, 4000},
    {ROCKET, 15000},
    {STEALTH, 24000},
    {SUB, 20000},
    {TCOPTER, 5000},
    {TANK, 7000},
};
