#pragma once
#include <SFML/Graphics.hpp>
#include <cmath>
#include <functional>
#include <resource_loader.hpp>
namespace UI
{
    class PartsMenuElement
    {
    public:
        PartsMenuElement(const std::string& text, const std::function<void()>& select);

        void executeChildFunction();

        void draw(sf::RenderWindow &window, const float& pos_x, const float& pos_y,const bool& selected);

        std::function<void()> on_selected;
        sf::Text text;
    };
}
