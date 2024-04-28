#pragma once

#include <engine/window_manager.hpp>
#include <helper/resource_loader.hpp>
#include <engine/scene.hpp>
#include <ui/button.hpp>
#include <network/network_manager.hpp>
#include <iostream>
#include <scenes/lobby.hpp>

namespace Scene
{
    class Menu : public Scene
    {
    public:
        Menu(WindowManager &window_manager, network::NetworkManager &network_manager, Lobby& lobby);

        virtual void run() override;

        void createGame();

    private:
        network::NetworkManager &network_manager;
        Lobby& lobby;
        sf::Font &title_font;

        UI::Button play_button{"Create Game", {20_percent, 20_percent, 70_percent, 20_percent}, [this]()
                               {lobby.setMode(Lobby::Create); 
                                setScene("lobby"); }};
        UI::Button editor_button{"Build", {40_percent, 20_percent, 50_percent, 20_percent}, [&]()
                               {lobby.setMode(Lobby::Join); 
                                setScene("lobby"); }};
        UI::Button exit_button{"Exit", {60_percent, 20_percent, 30_percent, 20_percent}, [&]()
                               { window_manager.close(); }};
    };
}