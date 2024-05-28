#pragma once
#include <vector>
#include <string>
#include <SFML/Graphics.hpp>
#include <ui/layout.hpp>
#include <engine/window_manager.hpp>
#include <helper/resource_loader.hpp>
#include <game/unit.hpp>

namespace UI
{
    class Header
    {
    public:
        Header();

        void draw(sf::RenderWindow &window);
        void setName(const std::string& name);
        void setMoney(const unsigned& money_count);

        sf::Text money, player;
        sf::Sprite money_sprite, playername_sprite;
    };
}