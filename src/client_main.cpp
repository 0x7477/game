

#include <network/client.hpp>
#include <network/network_manager.hpp>
#include <scenes/game.hpp>
#include <scenes/battle.hpp>

#include <engine/window_manager.hpp>
#include <scenes/menu.hpp>
#include <scenes/editor.hpp>
#include <X11/Xlib.h>
#include <iterator>
#include <fstream>

int main(int argc, char **argv)
{
    std::ifstream ifs("maps/raw/all_tiles.txt");
    const std::string map_data(std::istreambuf_iterator<char>{ifs}, {});

    
    XInitThreads();
    WindowManager window_manager{};
    Scene::Menu menu{window_manager};
    Scene::Battle map{window_manager,map_data};

    window_manager.scene = &map;

    window_manager.start();
    // network::Client client{"127.0.0.1", 2000};
    // network::NetworkManager manager{client};
    // Game game{manager};
    // game.run();

    return 0;
}