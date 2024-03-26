#include <ui/button.hpp>

template <>
void UI::Button::display<UI::Button::State::Normal>(sf::RenderWindow &window)
{
    const auto& rect = layout.getRect();
    background.setSize({rect.width, rect.height});
    background.setPosition(rect.x, rect.y);
    background.setOutlineThickness(0);
    window.draw(background);
}

template <>
void UI::Button::display<UI::Button::State::Hover>(sf::RenderWindow &window)
{
    const auto& rect = layout.getRect();
    background.setSize({rect.width, rect.height});
    background.setOutlineColor(sf::Color::Black);
    background.setOutlineThickness(10);
    background.setPosition(rect.x, rect.y);
    window.draw(background);
}

template <>
void UI::Button::display<UI::Button::State::Pressed>(sf::RenderWindow &window)
{
    display<Hover>(window);
}

void UI::Button::draw(sf::RenderWindow &window)
{
    const auto mouse_pos = sf::Mouse::getPosition(window);

    const auto hover = layout.getRect().contains(mouse_pos.x, mouse_pos.y);
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