#pragma once
#include <stdint.h>
#include "tile.hpp"
#include "direction.hpp"

namespace wfc
{
struct Rule
{
    Rule(const Tile &me, const Tile &target, const Direction &direction)
        : me{me}, target{target}, direction{direction}
    {
    }

    auto operator<=>(const Rule&) const = default;

    bool isDirectionValid(const unsigned int& x,const unsigned int& y,const unsigned int& dim_x,const unsigned int& dim_y) const
    {
        const auto x_pos{x + direction.x};
        const auto y_pos{y + direction.y};
        if(x_pos < 0) return false;
        if(x_pos >= dim_x) return false;

        if(y_pos < 0) return false;
        if(y_pos >= dim_y) return false;

        return true;

    }

    Tile me, target;
    Direction direction;
};

std::ostream& operator<<(std::ostream& os, const Rule& rule)
{
    return os << "(" << rule.me << " -> " << rule.target << ")";
}
}