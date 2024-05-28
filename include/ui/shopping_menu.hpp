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
    class ShoppingMenu
    {
    public:
        ShoppingMenu();

        bool canBuyUnit(const std::string &unit);

        void draw(sf::RenderWindow &window);

        void setOptions(const unsigned &money_, const std::vector<std::string> &new_options, const std::function<void(const std::size_t &)> &on_value_selected_function, const std::function<void()> &on_cancel_function);

        bool drawing{false};
        Layout layout;

        unsigned money{0};
        std::vector<std::string> options;
        std::size_t current_index{0};
        std::function<void(const std::size_t &)> on_value_selected{[](const std::size_t &) {}};
        std::function<void()> on_cancel{[]() {}};
        sf::Text unit_text, price_text;
        sf::Sprite background;
    };
}