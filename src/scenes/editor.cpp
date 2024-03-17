#include <scenes/editor.hpp>
#include <iostream>
#include "resource_loader.hpp"
#include <cmath>
Scene::Editor::Editor(WindowManager &window_manager)
    : Scene{"editor", window_manager},
      hexagon{ResourceLoader<sf::Texture>::get("hexagon.png")},
      hexagon_white{ResourceLoader<sf::Texture>::get("hexagon_white.png")}
{
}

void Scene::Editor::drawGrid()
{
    auto &window = window_manager.window;
    sf::Sprite hex_sprite{hexagon};
    sf::Sprite cursor{hexagon_white};

    hex_sprite.setScale(scroll_level / hexagon.getSize().x, scroll_level / hexagon.getSize().y);
    cursor.setScale(scroll_level / hexagon.getSize().x, scroll_level / hexagon.getSize().y);

    float offset = scroll_level / 2;

    for (auto y{-2}; y - 2 < (window.getSize().y / scroll_level) / 0.75; y++)
    {
        offset = offset == 0 ? scroll_level / 2 : 0;
        for (auto x{-2}; x - 2 < window.getSize().x / scroll_level; x++)
        {
            hex_sprite.setPosition(fmodf(view_x, scroll_level) + x * scroll_level + offset, (1.5) * fmodf(view_y / 1.5, scroll_level) + (0.75) * y * scroll_level);
            window.draw(hex_sprite);
        }
    }

    auto cursor_x = sf::Mouse::getPosition(window).x;
    auto cursor_y = sf::Mouse::getPosition(window).y;

    const auto view_offset_x = fmodf(view_x, scroll_level);
    const auto view_offset_y = fmodf(view_y, scroll_level * 0.75);

    auto y_index = int((cursor_y - view_offset_y) / (scroll_level * 0.75));

    bool even_row = (int)(view_y / (scroll_level * .75)) % 2;

    bool x_index_offset{0};
    const bool upper_part = fmodf(cursor_y - view_offset_y, (scroll_level * 0.75)) < scroll_level * 0.25;

    if (upper_part)
    {
        const float x = fmodf(cursor_x- view_offset_x - (y_index % 2+ (int)even_row) * scroll_level/2, scroll_level) / scroll_level;
        const float y = 1-fmodf(cursor_y - view_offset_y,scroll_level * 0.75) / (scroll_level * 0.25);

        const bool above = y >= -2 * std::abs(x-0.5)+1;
        if(above)
        {
            y_index --;
            if(x > 0.5)
                x_index_offset = 1;
        }
    }

    sf::RectangleShape box({scroll_level, scroll_level * 0.75f});
    box.setOutlineThickness(1);
    box.setOutlineColor(sf::Color::Blue);
    box.setFillColor(sf::Color::Transparent);

    const auto x_pos = cursor_x - fmodf(cursor_x -view_offset_x + (y_index %2 + (int)even_row) *scroll_level * 0.5 + (int)x_index_offset*scroll_level, scroll_level) ;
    const auto y_pos = y_index * scroll_level * 0.75 + view_offset_y;

    cursor.setPosition(x_pos, y_pos);
    window.draw(cursor);
}


void Scene::Editor::scrollView()
{
    const auto mouse_pos = sf::Mouse::getPosition(window_manager.window);
    static bool was_pressed{false};
    static int last_mouse_pos_x{0}, last_mouse_pos_y{0};

    float scroll_delta{0};

    if (WindowManager::keys[sf::Keyboard::Up])
        scroll_delta += time_delta;
    if (WindowManager::keys[sf::Keyboard::Down])
        scroll_delta -= time_delta;

    const float scroll_speed = 100;

    scroll_delta += WindowManager::mouse_delta * time_delta * scroll_speed;

    scroll_delta *= scroll_level * 0.1;

    const auto center_x = (-view_x + WindowManager::window_width /2) / scroll_level;
    const auto center_y = (-view_y + WindowManager::window_height/2) / scroll_level;

    view_x -= center_x * scroll_delta;
    view_y -= center_y * scroll_delta;

    scroll_level += scroll_delta;
    scroll_level = std::clamp(scroll_level, 10.f, 100.f);

    if (sf::Mouse::isButtonPressed(sf::Mouse::Button::Left))
    {
        if (was_pressed)
        {
            view_x += (mouse_pos.x - last_mouse_pos_x);
            view_y += (mouse_pos.y - last_mouse_pos_y);
        }

        last_mouse_pos_x = mouse_pos.x;
        last_mouse_pos_y = mouse_pos.y;
    }

    was_pressed = sf::Mouse::isButtonPressed(sf::Mouse::Button::Left);
}

void Scene::Editor::run()
{
    auto &window = window_manager.window;
    time_delta.update();
    scrollView();

    window.clear(sf::Color(35, 45, 55));

    drawGrid();
    window.display();
}
