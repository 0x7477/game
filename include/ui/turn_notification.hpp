#pragma once
#include <SFML/Graphics.hpp>
#include <helper/delta_timer.hpp>
namespace UI
{
    class TurnNotification
    {
    public:
        TurnNotification();
        
        void draw(sf::RenderWindow &window);
        void notify();

        bool show{false};
        sf::Text text;
        sf::Sprite background;

        float time{0};
        DeltaTimer delta_time;
    };
}