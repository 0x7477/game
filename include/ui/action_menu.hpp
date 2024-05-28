#pragma once
#include <vector>
#include <string>
#include <SFML/Graphics.hpp>
#include <ui/layout.hpp>
#include <engine/window_manager.hpp>
#include <helper/resource_loader.hpp>

namespace UI
{
    class ActionMenu
    {
    public:
        ActionMenu();
        

        void draw(sf::RenderWindow &window);

        void clearOptions();

        void setOptions(const std::vector<std::string> &new_options, const std::function<void(const std::size_t&)> &on_value_selected_function,const std::optional<std::function<void()>> &on_cancel_function);

        bool drawing{false};

        bool can_cancel{true};
        std::vector<std::string> options;
        std::size_t current_index{0};
        std::function<void(const std::size_t&)> on_value_selected{[](const std::size_t&){}};
        std::function<void()> on_cancel{[](){}};
        sf::Text action_text;
        sf::Sprite background;
    };
}