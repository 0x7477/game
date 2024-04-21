#pragma once
#include <string>
#include <SFML/Graphics.hpp>
#include <helper/resource_loader.hpp>
#include "layout.hpp"
namespace UI
{
    class Button
    {
    public:
        Button(const std::string &text_, const Layout &layout, std::function<void()> on_click);

        enum State
        {
            Normal,
            Hover,
            Pressed
        };

        template <State state = Normal>
        void display(sf::RenderWindow &window);

        void draw(sf::RenderWindow &window);

        Layout &getLayout()
        {
            return layout;
        }

        void setColor(const sf::Color &color)
        {
            background_color = color;
            background.setFillColor(background_color);
        }

        bool hover;
        sf::Color background_color{sf::Color::Transparent};
        sf::RectangleShape background;
        sf::Text text;

        Layout layout;
        std::function<void()> on_click;
    };
}

