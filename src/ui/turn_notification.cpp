#include <ui/turn_notification.hpp>
#include <engine/window_manager.hpp>
#include <helper/resource_loader.hpp>
UI::TurnNotification::TurnNotification()
: text{"", font_resources.get("arial.ttf"), 50}, background{image_resources.get("misc/menu.png")}
{
    const float x{200};
    const float y{100};
    text.setFillColor(sf::Color::Black);
    text.setString("Your\nTurn");

    sf::FloatRect textRect = text.getLocalBounds();
text.setOrigin(textRect.left + textRect.width/2.0f,
               textRect.top  + textRect.height/2.0f);
    background.scale(x / (float)background.getTexture()->getSize().x, y / (float)background.getTexture()->getSize().y);
}
void UI::TurnNotification::draw(sf::RenderWindow &window)
{
    delta_time.update();
    time += delta_time;

    if(time > 1)
        return;

    const auto pos_x = WindowManager::window_width / 2 - 100;
    const auto pos_y = WindowManager::window_height / 2 - 50;

    background.setPosition(pos_x, pos_y);
    text.setPosition(WindowManager::window_width / 2, WindowManager::window_height / 2);
    window.draw(background);
    window.draw(text);

}
void UI::TurnNotification::notify()
{
    delta_time.update();
    std::cout << "notify\n";
    time = 0;
}
