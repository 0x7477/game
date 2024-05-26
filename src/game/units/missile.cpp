#include <game/unit.hpp>
#include <game/map.hpp>
#include <game/game.hpp>
namespace Units
{
    class Missile : public Unit
    {
    public:
        Missile(const Team &team)
            : Unit{team, {4, Treads, 6, 3,5}}
        {
        }
    };

}
namespace
{
    volatile bool init = init_unit<Units::Missile>();
}