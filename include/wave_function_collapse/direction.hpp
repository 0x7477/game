#pragma once
#include <cstdint>

namespace wfc
{

    class Direction
    {
    public:
        constexpr Direction(const int8_t &x, const int8_t &y)
            : x{x}, y{y}
        {
            if (x == 0 && y == 1)
                direction = 0;
            else if (x == 1 && y == 1)
                direction = 1;
            else if (x == 1 && y == 0)
                direction = 2;
            else if (x == 1 && y == -1)
                direction = 3;
            else if (x == 0 && y == -1)
                direction = 4;
            else if (x == -1 && y == -1)
                direction = 5;
            else if (x == -1 && y == 0)
                direction = 6;
            else if (x == -1 && y == 1)
                direction = 7;
        }

        constexpr operator uint8_t() const
        {
            return direction;
        }
        int8_t x, y;
        uint8_t direction : 3;
    };
}