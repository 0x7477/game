#include <game/tile_index.hpp>
#include <network/datagram.hpp>
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


void serialize(std::vector<char> &buffer, const TileIndex &value)
{
    network::serialize_element(buffer, std::get<0>(value));
    network::serialize_element(buffer, std::get<1>(value));
}

void deserialize(std::string_view &buffer, TileIndex &value)
{
    network::deserialize_element(buffer, std::get<0>(value));
    network::deserialize_element(buffer, std::get<1>(value));
}