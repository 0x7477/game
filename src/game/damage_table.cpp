#include <game/attack_simulator.hpp>


std::map<std::tuple<std::string,std::string>, AttackSimulator::DamageValues> AttackSimulator::damage_table
{
    {{"Infantry","AntiAir"}, {{}, 5}},
    {{"Infantry","APC"}, {{}, 12}},
    {{"Infantry","Artillery"}, {{}, 15}},
    {{"Infantry","BattleCopter"}, {{}, 7}},
    {{"Infantry","Infantry"}, {{}, 55}},
    {{"Infantry","MediumTank"}, {{}, 1}},
    {{"Infantry","Mech"}, {{}, 45}},
    {{"Infantry","MegaTank"}, {{}, 1}},
    {{"Infantry","Missile"}, {{}, 26}},
    {{"Infantry","NeoTank"}, {{}, 1}},
    {{"Infantry","Piperunner"}, {{}, 5}},
    {{"Infantry","Recon"}, {{}, 12}},
    {{"Infantry","Rocket"}, {{}, 25}},
    {{"Infantry","TransportCopter"}, {{}, 30}},
    {{"Infantry","Tank"}, {{}, 5}},
};