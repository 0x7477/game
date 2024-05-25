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
    class UnitDetail
    {
    public:
        UnitDetail()
            : layout{}, text{"", font_resources.get("arial.ttf")}
        {
        }

        void draw(sf::RenderWindow &window)
        {
            text.setString(health);
            text.setPosition(500,500);
            window.draw(text);
            text.setString(ammo);
            text.setPosition(500,550);
            window.draw(text);
        }

        void setInfo(const unsigned &health_, const unsigned &ammo_)
        {
            health = std::to_string(health_);
            ammo = std::to_string(ammo_);
        }

        Layout layout;
        std::string health, ammo{0};
        sf::Text text;
    };
}