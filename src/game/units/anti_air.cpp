#include <game/unit.hpp>
#include <game/map.hpp>
#include <game/game.hpp>
namespace Units
{
    class AntiAir : public Unit
    {
    public:
        AntiAir(const Team &team)
            : Unit{team, {6, Treads, 9}}
        {
        }
    };
}
namespace
{
    volatile bool init = init_unit<Units::AntiAir>();
}