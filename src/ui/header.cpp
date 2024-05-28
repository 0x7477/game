#include <ui/header.hpp>

UI::Header::Header()
    : money{"", font_resources.get("arial.ttf"), 40}, player{"Kurt", font_resources.get("arial.ttf"), 40},
      money_sprite{image_resources.get("misc/money_banner.png")}, playername_sprite{image_resources.get("misc/top_banner.png")}
{
    playername_sprite.setScale(2, 2);
    money_sprite.setScale(2, 2);
    money.setPosition(10, -10);
    money.setFillColor(sf::Color::Black);
    player.setFillColor(sf::Color::Black);

}

void UI::Header::draw(sf::RenderWindow &window)
{
    window.draw(money_sprite);
    window.draw(money);

    playername_sprite.setPosition(WindowManager::window_width - playername_sprite.getGlobalBounds().width, 0);
    window.draw(playername_sprite);
    player.setPosition(WindowManager::window_width - player.getGlobalBounds().width - 10, -10);
    window.draw(player);

}

void UI::Header::setName(const std::string &)
{
    // player.setString(name);
}

void UI::Header::setMoney(const unsigned &money_count)
{
    money.setString(std::to_string(money_count));
}
