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

    static bool getKeyDown(const sf::Keyboard::Key& key);
    static bool getKey(const sf::Keyboard::Key& key);
    static unsigned int window_width, window_height;
    static int mouse_delta;
    void start();

    void close();

    void update();

    Scene::Scene* scene{nullptr};
    sf::RenderWindow window;
private:
    static bool keys[sf::Keyboard::KeyCount];
    static bool last_frame_keys[sf::Keyboard::KeyCount];
};