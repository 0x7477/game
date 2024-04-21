#pragma once
#include <tuple>
#include <iostream>

typedef std::tuple<unsigned, unsigned> TileIndex;

std::ostream& operator<< (std::ostream& os, const TileIndex& index);