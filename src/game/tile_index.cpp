#include <game/tile_index.hpp>
std::ostream& operator<< (std::ostream& os, const TileIndex& index)
{
    return os << "("<< std::get<0>(index) << "," << std::get<1>(index)<< ")";
}
