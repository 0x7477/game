#pragma once

#include <ui/gif.hpp>
#include <SFML/Graphics.hpp>
#include <game/tile_index.hpp>
#include <chrono>


class Map;

class Animation
{
    public:
    Animation(const UI::GIF& gif, const float &ttl,const TileIndex& index, const unsigned& anchor_x,const unsigned& anchor_y);

    bool draw(sf::RenderWindow &window, Map &map);

    TileIndex index;
    float ttl;
    UI::GIF gif;
    std::chrono::system_clock::time_point start_point;
};