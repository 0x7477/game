#pragma once
#include <cmath>
#include "tile_image.hpp"

namespace wfc
{
class Pattern
{
    public:
    enum Rotation : uint8_t
    {
        DEG0 = 0b00,
        DEG90 = 0b01,
        DEG180 = 0b10,
        DEG270 = 0b11,
    };

    enum Reflection : uint8_t
    {
        None = 0b00,
        Vertical = 0b01,
        Horizontal = 0b10,
        VerticalAndHorizontal = Vertical | Horizontal,
    };

    struct Configuration
    {
        Rotation rotation : 2;
        Reflection reflection : 2;
    };


    Pattern(const std::vector<TileId>& tiles)
    :dimension{(std::size_t)std::sqrt(tiles.size())},tiles{tiles}, hash{calculateHash()}
    {

    }

    Pattern(){}
    Pattern(const TileImage &image, const std::size_t &dimension, const std::size_t &x_pos, const std::size_t &y_pos, const Configuration &)
    :dimension{dimension}
    {
        tiles.reserve(dimension * dimension);
        for (auto y{y_pos}; y < y_pos + dimension; y++)
            for (auto x{x_pos}; x < x_pos + dimension; x++)
                tiles.push_back(image[x, y]);

        hash = calculateHash();
    }

    std::size_t calculateHash() const
    {
        constexpr std::hash<std::string_view> hasher{};
        const std::string_view data{(char*)tiles.data(), tiles.size() * sizeof(TileId)};
        return hasher(data);
    }

    std::size_t getHash() const
    {
        return hash;
    }

    TileId operator [](std::size_t x, std::size_t y) const
    {
        return tiles[x + y * dimension]; 
    }

    void draw() const
    {
        for (unsigned int y = 0; y < dimension; y++)
        {
            for (unsigned int x = 0; x < dimension; x++)
                std::cout << (int)tiles[x + y * dimension] << " ";
            
            std::cout << std::endl;
        }
    }

private:
    std::size_t dimension; 
    std::vector<TileId> tiles;
    std::size_t hash; 
};
}