#include <game/units/trooper.hpp>
namespace Units
{
    class Mech : public Trooper
    {
    public:
        Mech(const Team &team)
            : Trooper{team, {2, MovementType::Mech, 3}}
        {
        }
    };

}
namespace
{
    volatile bool init = init_unit<Units::Mech>();
}