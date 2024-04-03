#pragma once

#include <engine/window_manager.hpp>
#include <helper/resource_loader.hpp>
#include <engine/scene.hpp>
#include <ui/button.hpp>
#include <iostream>


namespace Scene
{
    class Menu : public Scene
    {
    public:
        Menu(WindowManager &window_manager);

        virtual void run() override;

    private:
        sf::Font &title_font;


        UI::Button play_button{"Play", {20_percent, 20_percent, 70_percent, 20_percent}, [](){std::cout << "play\n";}};
        UI::Button editor_button{"Build", {40_percent, 20_percent, 50_percent, 20_percent}, [&](){setScene("editor");}};

        UI::Button exit_button{"Exit", {60_percent, 20_percent, 30_percent, 20_percent},[&](){window_manager.close();}};
    };
}
