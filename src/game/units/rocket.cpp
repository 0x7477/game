#include <game/unit.hpp>
namespace Units
{
    class Rocket : public Unit
    {
    public:
        Rocket(const Team &team)
            : Unit{team, {5, Treads, 6, 3,5}}
        {
        }
    };

}
namespace
{
    volatile bool init = init_unit<Units::Rocket>();
}