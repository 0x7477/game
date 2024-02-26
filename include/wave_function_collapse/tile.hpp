#pragma once
#include <cstdint>
#include <magic_enum.hpp>
#include <iostream>

namespace wfc
{
    const unsigned int maximum_number_of_states{255};
    typedef unsigned char entropy_t;

    enum TileType
    {
        Unknown = 0,
        Black,
        White
    };

    class Tile
    {
    public:
        Tile() : type{Unknown} {}
        Tile(const TileType &type) : type{type} {}
        Tile(const unsigned int &type) : type{(uint8_t)type} {}

        friend std::ostream &operator<<(std::ostream &os, const Tile &tile)
        {
            return os << (int)tile.type; // << magic_enum::enum_name(tile.type);
        }

        operator uint8_t() const
        {
            return (uint8_t)type;
        }

        uint8_t type;
    };
}