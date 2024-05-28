#pragma once
#include <tuple>
#include <iostream>
#include <vector>

class TileIndex
{
public:
    TileIndex();
    TileIndex(const unsigned &x, const unsigned &y);

    bool operator==(const TileIndex& other) const;
    bool operator<(const TileIndex& other) const;
    static bool areTileIndexesAdjacent(const TileIndex &a, const TileIndex &b);
    static unsigned distance(const TileIndex &a, const TileIndex &b);
    unsigned x{0}, y{0};
};

std::ostream & operator<<(std::ostream &os, const TileIndex &index);