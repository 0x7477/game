#include <game/unit.hpp>
namespace Units
{
    class Missile : public Unit
    {
    public:
        Missile(const Team &team)
            : Unit{team, {4, Tires, 6, 3,5}}
        {
        }
    };

}
namespace
{
    volatile bool init = init_unit<Units::Missile>();
}