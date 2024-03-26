#pragma once
#include <SFML/Graphics.hpp>
#include "layout.hpp"

namespace UI
{
    class Panel
    {
    public:
        Panel(const Layout &layout)
            : layout{layout} {}

        void draw(sf::RenderWindow &window)
        {
            const auto &rect = layout.getRect();
            sf::RectangleShape background(sf::Vector2f(rect.width, rect.height));
            background.setFillColor(background_color);
            background.setPosition(rect.x, rect.y);
            window.draw(background);
        }

        Layout &getLayout()
        {
            return layout;
        }

        void setColor(const sf::Color &color)
        {
            background_color = color;
        }

        sf::Color background_color{sf::Color::Transparent};
        Layout layout;
    };
}
