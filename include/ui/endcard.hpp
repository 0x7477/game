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
        EndCard()
            : layout{}, result_text{"",font_resources.get("arial.ttf")}
        {
            result_text.setFillColor(sf::Color::Black);
        }
        

        void draw(sf::RenderWindow &window)
        {

            if(WindowManager::getKeyDown(sf::Keyboard::Y))
            {
                on_exit();
                return;
            }

            sf::RectangleShape background;

            const auto rect = layout.getRect();
            background.setPosition(rect.x, rect.y);
            background.setSize({rect.width, rect.height});

            window.draw(background);

            result_text.setPosition(layout.getRect().x + layout.getRect().width/2,layout.getRect().y + layout.getRect().height/2);
            window.draw(result_text);

            
        }

        void setResult(const bool& win, std::function<void()> on_exit_)
        {
            result_text.setString(win? "win": "defeat");
            on_exit = on_exit_;
        }

        Layout layout;
        std::function<void()> on_exit{[](){}};
        sf::Text result_text;
    };
}