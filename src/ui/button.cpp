#include <ui/button.hpp>

template <>
void UI::Button::display<UI::Button::State::Normal>(sf::RenderWindow &window)
{
    const auto& rect = layout.getRect();
    sf::RectangleShape background(sf::Vector2f(rect.width, rect.height));
    background.setFillColor(background_color);
    background.setPosition(rect.x, rect.y);
    window.draw(background);
}

template <>
void UI::Button::display<UI::Button::State::Hover>(sf::RenderWindow &window)
{
    display<Normal>(window);
}

template <>
void UI::Button::display<UI::Button::State::Pressed>(sf::RenderWindow &window)
{
    display<Hover>(window);
}

void UI::Button::draw(sf::RenderWindow &window)
{
    const auto mousePos = sf::Mouse::getPosition(window);

    const auto hover = layout.getRect().contains(mousePos.x, mousePos.y);
    const auto clicked = sf::Mouse::isButtonPressed(sf::Mouse::Button::Left);

    if (!hover)
        display<Normal>(window);
    else if (!clicked)
        display<Hover>(window);
    else
    {
        on_click();
        display<Pressed>(window);
    }
}