#include <engine/window_manager.hpp>
#include <iostream>
#include <thread>
#include <engine/scene.hpp>

bool WindowManager::keys[sf::Keyboard::KeyCount]{};
bool WindowManager::last_frame_keys[sf::Keyboard::KeyCount]{};
unsigned int WindowManager::window_width{0};
unsigned int WindowManager::window_height{0};
int WindowManager::mouse_delta{0};

WindowManager::WindowManager()
    : window(sf::VideoMode(800, 600), "", sf::Style::Default)
{
    window_width = window.getSize().x;
    window_height = window.getSize().y;
}

void WindowManager::start()
{
    while (window.isOpen())
        update();
}
void WindowManager::update()
{
    sf::Event event;
    mouse_delta = 0;
    while (window.pollEvent(event))
    {
        if (event.type == sf::Event::Closed || (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Escape))
            window.close();

        else if (event.type == sf::Event::KeyPressed)
            keys[event.key.code] = true;

        else if (event.type == sf::Event::KeyReleased)
            keys[event.key.code] = false;

        else if (event.type == sf::Event::Resized)
        {
            window_width = event.size.width;
            window_height = event.size.height;
            window.setView(sf::View(sf::FloatRect(0, 0, window_width, window_height)));
            scene->resize();
        }
        else if(event.type == sf::Event::MouseWheelMoved)
        {
            mouse_delta = event.mouseWheel.delta;
        }
    }

    if (scene)
        scene->run();
        
    for(auto i{0u}; i< sf::Keyboard::KeyCount; i++ )
        last_frame_keys[i] = keys[i];
}

bool WindowManager::getKeyDown(const sf::Keyboard::Key& key)
{
    if(!keys[key])
        return false;

    const bool first_press_this_frame = keys[key] && !last_frame_keys[key];

    if(first_press_this_frame)
        last_frame_keys[key] = true; // dont return getKeyDown true twice in a frame

    return first_press_this_frame;
}
bool WindowManager::getKey(const sf::Keyboard::Key& key)
{
    return keys[key];
}

void WindowManager::close()
{
    window.close();
}