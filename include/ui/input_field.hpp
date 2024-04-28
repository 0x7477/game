#pragma once
#include <string>
#include <SFML/Graphics.hpp>
#include <helper/resource_loader.hpp>
#include "layout.hpp"
namespace UI
{
    class InputField
    {
    public:
        InputField(const Layout &layout);

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

        std::string getValue() const;

        sf::Color background_color{sf::Color::Blue};
        sf::RectangleShape background;
        sf::Text text;
        std::string value{};

        Layout layout;
    };
}

