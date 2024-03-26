

#include <network/client.hpp>
#include <network/network_manager.hpp>
#include <scenes/game.hpp>

#include "window_manager.hpp"
#include <scenes/menu.hpp>
#include <scenes/editor.hpp>
#include <X11/Xlib.h>
#include <spaceship_part.hpp>

int main(int argc, char **argv)
{

    
    XInitThreads();
    WindowManager window_manager{};
    Scene::Menu menu{window_manager};
    Scene::Editor build{window_manager};

    window_manager.scene = &menu;

    window_manager.start();
    // network::Client client{"127.0.0.1", 2000};
    // network::NetworkManager manager{client};
    // Game game{manager};
    // game.run();

    return 0;
}