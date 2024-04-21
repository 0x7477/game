#pragma once
#include <array>
#include <tuple>

namespace wfc
{

constexpr std::array reverse_directions{3, 2, 1, 0};

const std::array directions{std::tuple{0, -1},
                            std::tuple{-1, 0},
                            std::tuple{1, 0},
                            std::tuple{0, 1}};

}