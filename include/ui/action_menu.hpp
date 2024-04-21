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
        ActionMenu()
            : layout{10_percent, 10_percent, 70_percent, 50_percent}, action_text{"",font_resources.get("arial.ttf")}
        {
        }
        

        void draw(sf::RenderWindow &window)
        {
            if(WindowManager::getKeyDown(sf::Keyboard::X))
            {
                on_cancel();
                return;
            }

            if(WindowManager::getKeyDown(sf::Keyboard::Y))
            {
                on_value_selected(current_index);
                return;
            }

            if(options.size() == 0)
                return;

            if(WindowManager::getKeyDown(sf::Keyboard::Down))
                current_index = (current_index +1 ) % options.size();

            if(WindowManager::getKeyDown(sf::Keyboard::Up))
                current_index = (current_index -1 +options.size()) % options.size();

            
            
            sf::RectangleShape background;

            const auto rect = layout.getRect();
            background.setPosition(rect.x, rect.y);
            background.setSize({rect.width, rect.height});

            window.draw(background);

            for(auto i{0u}; i < options.size(); i++)
            {
                action_text.setFillColor(i == current_index ?sf::Color::Red : sf::Color::Black);
                action_text.setPosition(layout.getRect().x,layout.getRect().y + i*30);
                action_text.setString(options[i]);
                window.draw(action_text);
            }
        }

        void clearOptions()
        {
            options.clear();
        }

        void setOptions(const std::vector<std::string> &new_options, const std::function<void(const std::size_t&)> &on_value_selected_function,const std::function<void()> &on_cancel_function)
        {
            options = new_options;
            on_value_selected = on_value_selected_function;
            on_cancel = on_cancel_function;
            current_index = 0;
        }

        bool drawing{false};
        Layout layout;


        std::vector<std::string> options;
        std::size_t current_index{0};
        std::function<void(const std::size_t&)> on_value_selected{[](const std::size_t&){}};
        std::function<void()> on_cancel{[](){}};
        sf::Text action_text;
    };
}