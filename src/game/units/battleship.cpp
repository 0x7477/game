#include <game/unit.hpp>
namespace Units
{
    class Battleship : public Unit
    {

    public:
        Battleship(const Team &team)
            : Unit{team, {5,Ship,9,2,6}}
        {}
    };
}

namespace
{
    volatile bool init = init_unit<Units::Battleship>();
}