#pragma once

#include <engine/window_manager.hpp>
#include <helper/resource_loader.hpp>
#include <engine/scene.hpp>
#include <ui/button.hpp>
#include <ui/input_field.hpp>
#include <iostream>
#include <network/network_manager.hpp>
#include <game/lobby.hpp>
#include <scenes/battle.hpp>

namespace Scene
{
    class Lobby : public Scene
    {
    public:

        enum Mode
        {
            Create, Join
        };

        Lobby(WindowManager &window_manager, network::NetworkManager &network_manager, Battle& battle);

        virtual void run() override;

        void setLobbyId(const network::Datagram<std::string>& id);
        std::string getPlayerName();

        void notifyPlayerJoined(const LobbyMember &member);
        void drawLobby();
        void displayJoinError(const std::string& error);
        void promptCode();
        void joinGame();
        void sendReady();
        void setReady(const unsigned& id);
        void setMode(const Mode& mode);
        void setId(const std::string& id_);
        void onJoined(const std::optional<std::string>& error);
    private:

        std::string username{};
        Mode mode;
        std::string id{};
        network::NetworkManager &network_manager;
        Battle& battle;
        bool joined{false};
        bool connecting{false};
        bool ready{false};

        std::optional<std::string> error{};
        sf::Text title_text;
        sf::Text room_number;
        UI::Layout player_area{30_percent, 20_percent, 45_percent, 20_percent};
        UI::Button error_button{"Error", {20_percent, 20_percent, 40_percent, 20_percent},[&](){error={};}};
        UI::InputField lobby_id_input{{20_percent, 20_percent, 60_percent, 20_percent}};
        UI::Button ready_button{"Ready", {60_percent, 20_percent, 30_percent, 60_percent}, [&](){ready = true; sendReady();}};
        UI::Button join_button{"Join", {60_percent, 20_percent, 30_percent, 60_percent}, [&](){joinGame();}};
        UI::Button exit_button{"Exit", {60_percent, 60_percent, 30_percent, 20_percent},[&](){setScene("menu");}};
        sf::Sprite background, player_background;
    };

    
}

inline std::map<std::string, Lobby> lobbies{};
inline Lobby lobby;
inline Scene::Lobby* lobby_scene;