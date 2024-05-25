#include <game/attack_simulator.hpp>
#include <game/unit_names.hpp>

std::map<std::tuple<std::string, std::string>, AttackSimulator::DamageValues> AttackSimulator::damage_table{

    {{ANTIAIR, ANTIAIR}, {45, {}}},
    {{ANTIAIR, APC}, {50, {}}},
    {{ANTIAIR, ARTILLERY}, {50, {}}},
    {{ANTIAIR, BATTLECOPTER}, {120, {}}},
    {{ANTIAIR, BOMBER}, {75, {}}},
    {{ANTIAIR, FIGHTER}, {65, {}}},
    {{ANTIAIR, INFANTRY}, {105, {}}},
    {{ANTIAIR, MEDIUMTANK}, {10, {}}},
    {{ANTIAIR, MECH}, {105, {}}},
    {{ANTIAIR, MEGATANK}, {105, {}}},
    {{ANTIAIR, MISSILE}, {55, {}}},
    {{ANTIAIR, NEOTANK}, {5, {}}},
    {{ANTIAIR, PIPERUNNER}, {25, {}}},
    {{ANTIAIR, RECON}, {60, {}}},
    {{ANTIAIR, ROCKET}, {55, {}}},
    {{ANTIAIR, STEALTH}, {75, {}}},
    {{ANTIAIR, TCOPTER}, {120, {}}},
    {{ANTIAIR, TANK}, {120, {}}},


    // ARTILLERY
    {{ARTILLERY, ANTIAIR}, {75, {}}},
    {{ARTILLERY, APC}, {70, {}}},
    {{ARTILLERY, ARTILLERY}, {75, {}}},
    {{ARTILLERY, BATTLESHIP}, {40, {}}},
    {{ARTILLERY, BLACKBOAT}, {55, {}}},
    {{ARTILLERY, CARRIER}, {45, {}}},
    {{ARTILLERY, BATTLESHIP}, {65, {}}},
    {{ARTILLERY, INFANTRY}, {90, {}}},
    {{ARTILLERY, LANDER}, {55, {}}},
    {{ARTILLERY, MEDIUMTANK}, {45, {}}},
    {{ARTILLERY, MECH}, {85, {}}},
    {{ARTILLERY, MEGATANK}, {15, {}}},
    {{ARTILLERY, MISSILE}, {80, {}}},
    {{ARTILLERY, NEOTANK}, {40, {}}},
    {{ARTILLERY, PIPERUNNER}, {70, {}}},
    {{ARTILLERY, RECON}, {80, {}}},
    {{ARTILLERY, ROCKET}, {80, {}}},
    {{ARTILLERY, SUB}, {60, {}}},
    {{ARTILLERY, TANK}, {70, {}}},

    // INFANTRY
    {{INFANTRY, ANTIAIR}, {{}, 5}},
    {{INFANTRY, APC}, {{}, 12}},
    {{INFANTRY, ARTILLERY}, {{}, 15}},
    {{INFANTRY, BATTLECOPTER}, {{}, 7}},
    {{INFANTRY, INFANTRY}, {{}, 55}},
    {{INFANTRY, MEDIUMTANK}, {{}, 1}},
    {{INFANTRY, MECH}, {{}, 45}},
    {{INFANTRY, MEGATANK}, {{}, 1}},
    {{INFANTRY, MISSILE}, {{}, 26}},
    {{INFANTRY, NEOTANK}, {{}, 1}},
    {{INFANTRY, PIPERUNNER}, {{}, 5}},
    {{INFANTRY, RECON}, {{}, 12}},
    {{INFANTRY, ROCKET}, {{}, 25}},
    {{INFANTRY, TCOPTER}, {{}, 30}},
    {{INFANTRY, TANK}, {{}, 5}},
    // TANK
    {{TANK, ANTIAIR}, {65, 5}},
    {{TANK,APC}, {75, 54}},
    {{TANK,ARTILLERY}, {70, 45}},
    {{TANK,BATTLECOPTER}, {{}, 10}},
    {{TANK,BATTLESHIP}, {1, {}}},
    {{TANK,BLACKBOAT}, {10, {}}},
    {{TANK,CARRIER}, {1, {}}},
    {{TANK,CRUISER}, {5, {}}},
    {{TANK,INFANTRY}, {{}, 75}},
    {{TANK,LANDER}, {10, {}}},
    {{TANK,MEDIUMTANK}, {15, 1}},
    {{TANK,MECH}, {{}, 70}},
    {{TANK,MEGATANK}, {10, 1}},
    {{TANK,MISSILE}, {85, 30}},
    {{TANK,NEOTANK}, {15, 1}},
    {{TANK,PIPERUNNER}, {55, 6}},
    {{TANK,RECON}, {85, 40}},
    {{TANK,MISSILE}, {85, 55}},
    {{TANK,SUB}, {1, {}}},
    {{TANK,TCOPTER}, {{}, 40}},
    {{TANK,TANK}, {55, 6}}
};