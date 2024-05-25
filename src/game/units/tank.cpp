#include <game/unit.hpp>
#include <game/map.hpp>
#include <game/game.hpp>
namespace Units
{
    class Tank : public Unit
    {
    public:
        Tank(const Team &team)
            : Unit{team, {6, Treads, 5}}
        {
        }
    };

}
namespace
{
    volatile bool init = init_unit<Units::Tank>();
}