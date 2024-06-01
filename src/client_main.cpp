

#include <network/client.hpp>
#include <network/network_manager.hpp>
#include <scenes/battle.hpp>

#include <engine/window_manager.hpp>
#include <scenes/menu.hpp>
#include <scenes/editor.hpp>
#include <scenes/lobby.hpp>
#include <X11/Xlib.h>
#include <iterator>
#include <fstream>

int main(int, char **)
{
    network::Client client{"89.163.208.69", 2001};
    // network::Client client{"127.0.0.1", 2001};
    network::NetworkManager manager{client};

    std::ifstream ifs("maps/raw/mewtwo.txt");
    // std::ifstream ifs("maps/raw/just_plains.txt");
    const std::string map_data(std::istreambuf_iterator<char>{ifs}, {});

    XInitThreads();
    WindowManager window_manager{};
    Scene::Battle map{window_manager,manager, map_data};
    Scene::Lobby lobby{window_manager, manager, map};
    Scene::Menu menu{window_manager, manager, lobby};

    window_manager.scene = &menu;
    // window_manager.scene = &map;

    window_manager.start();
    // Game game{manager};
    // game.run();

    return 0;
}