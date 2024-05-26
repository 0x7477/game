#include <game/unit.hpp>
#include <game/map.hpp>
#include <game/game.hpp>
#include <game/units/transport.hpp>
#include <game/unit_names.hpp>
namespace Units
{
    class Lander : public Transport<2>
    {

    public:
        Lander(const Team &team)
            : Transport{team, {6, MovementType::Lander,0}, std::vector<std::string>{ANTIAIR,APC,ARTILLERY,INFANTRY,MEDIUMTANK,MECH,MISSILE,MEGATANK,NEOTANK,PIPERUNNER,RECON,ROCKET,TANK}}
        {}
    };
}

namespace
{
    volatile bool init = init_unit<Units::Lander>();
}