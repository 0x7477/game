#pragma once
#include <tuple>
#include <iostream>

typedef std::tuple<unsigned, unsigned> TileIndex;

bool areTileIndexesAdjacent(const TileIndex& a,const TileIndex& b);
unsigned distance(const TileIndex& a,const TileIndex& b);

std::ostream& operator<< (std::ostream& os, const TileIndex& index);