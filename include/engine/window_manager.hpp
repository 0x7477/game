#pragma once
#include <SFML/Graphics.hpp>

namespace Scene
{
    class Scene;
}
class WindowManager
{
public:
    WindowManager();

    static bool keys[sf::Keyboard::KeyCount];
    static unsigned int window_width, window_height;
    static int mouse_delta;
    void start();

    void close();

    void update();

    Scene::Scene* scene{nullptr};
    sf::RenderWindow window;
};