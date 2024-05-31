#pragma once
#include <SFML/Graphics.hpp>
#include <game/tile_index.hpp>
#include <game/attack_simulator.hpp>
class Map;
namespace UI
{
    class AttackInfo
    {
    public:
        AttackInfo();
        
        void draw(sf::RenderWindow &window);
        void update(Map & map, const TileIndex& index);

        bool show{false};
        AttackSimulator::AttackPossibilities attack_info;
        TileIndex last_index;
        sf::Text text;
        sf::Sprite background;
    };
}