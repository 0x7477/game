#pragma once
#include <SFML/Graphics.hpp>
#include <cmath>

#include <resource_loader.hpp>

#include <vector>
#include <variant>
#include "element.hpp"

namespace UI
{
    class PartsMenuRing;
    class PartsMenuCategory
    {
    public:
        PartsMenuCategory(const unsigned int& layer,const std::string &text, const std::vector<std::variant<PartsMenuCategory, PartsMenuElement>> &children);

        void draw(sf::RenderWindow &window, const float &center_x, const float &center_y, const float &pos_x, const float &pos_y, const bool &selected);
        void setLayer(const unsigned int& layer_);

        void executeChildFunction();
        unsigned int layer;
        constexpr static int font_size{10};
        std::vector<std::variant<PartsMenuCategory, PartsMenuElement>> children;
        sf::Text text;
        PartsMenuRing *ring;
    };
}
