#pragma once
#include <SFML/Graphics.hpp>
#include "layout.hpp"
#include <cmath>
#include "parts_menu/ring.hpp"
#include <functional>

namespace UI
{
    class PartsMenu
    {
    public:
        PartsMenu(const Layout &layout, const std::function<void(const std::string &)>& on_value_change);

        void draw(sf::RenderWindow &window);

        void set_id(const std::string &s);

        Layout &getLayout();

        bool drawing{false};
        Layout layout;
        PartsMenuRing ring;
        std::function < void(const std::string &)> on_value_change;
        std::string value;
    };
}
