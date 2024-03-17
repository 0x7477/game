#pragma once
#include <string>
#include <SFML/Graphics.hpp>
#include "layout.hpp"
namespace UI
{
    class Button
    {
    public:
        Button(const std::string &text, const Layout &layout, std::function<void()> on_click)
            : text{text}, layout{layout}, on_click{on_click} {}

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
        }

        bool hover;
        sf::Color background_color{sf::Color::Transparent};
        std::string text;
        Layout layout;
        std::function<void()> on_click;
    };
}

