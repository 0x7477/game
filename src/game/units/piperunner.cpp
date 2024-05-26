#include <game/unit.hpp>
#include <game/map.hpp>
#include <game/game.hpp>
namespace Units
{
    class Piperunner : public Unit
    {
    public:
        Piperunner(const Team &team)
            : Unit{team, {9, Pipe, 9, 2,5}}
        {
        }
    };

}
namespace
{
    volatile bool init = init_unit<Units::Piperunner>();
}