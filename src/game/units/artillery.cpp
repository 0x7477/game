#include <game/unit.hpp>
#include <game/map.hpp>
#include <game/game.hpp>
namespace Units
{
    class Artillery : public Unit
    {
    public:
        Artillery(const Team &team)
            : Unit{team, {5, Treads, 9, 2,3}}
        {
        }
    };

}
namespace
{
    volatile bool init = init_unit<Units::Artillery>();
}