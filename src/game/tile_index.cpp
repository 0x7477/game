#include <game/tile_index.hpp>
#include <network/datagram.hpp>


TileIndex::TileIndex(){};
TileIndex::TileIndex(const unsigned &x, const unsigned &y)
:x{x}, y{y}
{}

std::ostream &operator<<(std::ostream &os, const TileIndex &index)
{
    return os << "(" << index.x << "," << index.y << ")";
}

unsigned TileIndex::distance(const TileIndex &i1, const TileIndex &i2)
{
    return abs((int)i1.x - (int)i2.x) + abs((int)i1.y - (int)i2.y);
}

bool TileIndex::operator==(const TileIndex& other) const
{
    return std::tuple{x,y} == std::tuple{other.x,other.y};
}

bool TileIndex::operator<(const TileIndex& other) const
{
    return std::tuple{x,y} < std::tuple{other.x,other.y};
}


bool TileIndex::areTileIndexesAdjacent(const TileIndex &i1, const TileIndex &i2)
{
    return distance(i1, i2) == 1;
}