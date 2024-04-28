#include <ui/input_field.hpp>
#include <engine/window_manager.hpp>

UI::InputField::InputField(const Layout &layout)
    : text{"", font_resources.get("arial.ttf")}, layout{layout}
{
    background.setFillColor(background_color);
}

void UI::InputField::draw(sf::RenderWindow &window)
{
    if (WindowManager::getKeyDown(sf::Keyboard::Key::Num0))
        value += "0";
    if (WindowManager::getKeyDown(sf::Keyboard::Key::Num1))
        value += "1";
    if (WindowManager::getKeyDown(sf::Keyboard::Key::Num2))
        value += "2";
    if (WindowManager::getKeyDown(sf::Keyboard::Key::Num3))
        value += "3";
    if (WindowManager::getKeyDown(sf::Keyboard::Key::Num4))
        value += "4";
    if (WindowManager::getKeyDown(sf::Keyboard::Key::Num5))
        value += "5";
    if (WindowManager::getKeyDown(sf::Keyboard::Key::Num6))
        value += "6";
    if (WindowManager::getKeyDown(sf::Keyboard::Key::Num7))
        value += "7";
    if (WindowManager::getKeyDown(sf::Keyboard::Key::Num8))
        value += "8";
    if (WindowManager::getKeyDown(sf::Keyboard::Key::Num9))
        value += "9";

    const auto &rect = layout.getRect();
    background.setSize({rect.width, rect.height});
    background.setPosition(rect.x, rect.y);
    background.setOutlineThickness(0);
    window.draw(background);

    text.setString(value);
    sf::FloatRect float_rect = text.getLocalBounds();
    text.setOrigin(float_rect.left + float_rect.width / 2.0f, float_rect.top + float_rect.height / 2.0f);
    text.setPosition(rect.x + rect.width / 2, rect.y + rect.height / 2);
    window.draw(text);
}

std::string UI::InputField::getValue() const
{
    return value;
}