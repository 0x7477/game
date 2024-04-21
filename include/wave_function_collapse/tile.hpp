#pragma once
#include <cstdint>

namespace wfc
{

typedef uint8_t TileId; 

struct Tile
{
    Tile(TileId id)
    : id{id}{};

    operator TileId () const
    {
        return id;
    }

    operator int () const
    {
        return id;
    }

    TileId id;
};
}