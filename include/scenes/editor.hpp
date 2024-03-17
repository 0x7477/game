#pragma once

#include "window_manager.hpp"
#include "scene.hpp"
#include <SFML/Graphics.hpp>
#include <delta_timer.hpp>
#include <cmath>
namespace Scene
{
    class Editor : public Scene
    {
    public:
        Editor(WindowManager &window_manager);

        void scrollView();
        void drawGrid();
        virtual void run() override;

    private:
        float scroll_level{50};
        float view_x{10}, view_y{10};
        sf::Texture hexagon, hexagon_white;
        DeltaTimer time_delta{};
    };
}
