#pragma once
#include <tuple>
#include <iostream>
#include <vector>

typedef std::tuple<unsigned, unsigned> TileIndex;

void serialize(std::vector<char> &buffer, const TileIndex &value);
void deserialize(std::string_view &buffer, TileIndex &value);

bool areTileIndexesAdjacent(const TileIndex& a,const TileIndex& b);
unsigned distance(const TileIndex& a,const TileIndex& b);

std::ostream& operator<< (std::ostream& os, const TileIndex& index);