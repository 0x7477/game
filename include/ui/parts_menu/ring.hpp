#pragma once
#include <SFML/Graphics.hpp>
#include <vector>
#include <variant>

#include "category.hpp"
#include "element.hpp"
namespace UI
{
    class PartsMenuRing
    {
    public:
        PartsMenuRing(const unsigned int & layer,  const std::vector<std::variant<PartsMenuCategory, PartsMenuElement>> &children);

        void draw(sf::RenderWindow &window, const float &pos_x, const float &pos_y);

        void executeChildFunction();

        int selected_index = -1;
        float radius,thickness;
        std::vector<std::variant<PartsMenuCategory, PartsMenuElement>> children;
    };
}
