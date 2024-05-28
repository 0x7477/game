#pragma once
#include <vector>
#include <string>
#include <SFML/Graphics.hpp>
#include <ui/layout.hpp>
#include <engine/window_manager.hpp>
#include <helper/resource_loader.hpp>

namespace UI
{
    class EndCard
    {
    public:
        EndCard();
        

        void draw(sf::RenderWindow &window);

        void setResult(const bool& win, std::function<void()> on_exit_);

        Layout layout;
        std::function<void()> on_exit{[](){}};
        sf::Text result_text;
    };
}