#pragma once
#include <vector>
#include <string>
#include <SFML/Graphics.hpp>
#include <ui/layout.hpp>
#include <engine/window_manager.hpp>
#include <helper/resource_loader.hpp>
#include <game/unit.hpp>

class Tile;

namespace UI
{
    class UnitDetail
    {
    public:
        UnitDetail();

        void draw(sf::RenderWindow &window);

        void setInfo(const Tile & tile);

        std::string protection, ammo, health;
        sf::Text text;
        sf::Sprite background,health_symbol, shield_symbol, ammo_symbol;
        bool display_ammo{false}, display_health{false};
    };
}