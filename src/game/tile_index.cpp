#include <game/tile_index.hpp>
std::ostream& operator<< (std::ostream& os, const TileIndex& index)
{
    return os << "("<< std::get<0>(index) << "," << std::get<1>(index)<< ")";
}


unsigned distance(const TileIndex& i1,const TileIndex& i2)
{
    const auto [x1,y1] = i1;
    const auto [x2,y2] = i2;
    return abs((int)x1 - (int)x2) + abs((int)y1 - (int)y2);
}

bool areTileIndexesAdjacent(const TileIndex& i1,const TileIndex& i2)
{
    return distance(i1,i2) == 1;
}
