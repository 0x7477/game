#include <ui/button.hpp>

UI::Button::Button(const std::string &text_, const Layout &layout, std::function<void()> on_click)
    :background{image_resources.get("misc/top_banner.png")}, text{text_, font_resources.get("arial.ttf"), 50}, layout{layout}, on_click{on_click}
{
    background.setColor(background_color);
    sf::FloatRect rect = text.getLocalBounds();
    text.setOrigin(rect.left + rect.width / 2.0f, rect.top + rect.height / 2.0f);
}

UI::Layout &UI::Button::getLayout()
{
    return layout;
}

void UI::Button::setColor(const sf::Color &color)
{
    background_color = color;
    background.setColor(background_color);
}

template <>
void UI::Button::display<UI::Button::State::Normal>(sf::RenderWindow &window)
{
    const auto &rect = layout.getRect();

    const auto &texture_rect = background.getTexture()->getSize();
    background.setScale(rect.width / texture_rect.x,rect.height / texture_rect.y);
    
    background.setPosition(rect.x, rect.y);
    window.draw(background);

    text.setPosition(rect.x + rect.width / 2, rect.y + rect.height / 2);
    window.draw(text);
}

template <>
void UI::Button::display<UI::Button::State::Hover>(sf::RenderWindow &window)
{
    const auto &rect = layout.getRect();

    const auto &texture_rect = background.getTexture()->getSize();
    background.setScale(rect.width / texture_rect.x,rect.height / texture_rect.y);
    
    background.setPosition(rect.x, rect.y + 5);
    window.draw(background);

    text.setPosition(rect.x + rect.width / 2, rect.y + rect.height / 2 + 5);
    window.draw(text);
}

template <>
void UI::Button::display<UI::Button::State::Pressed>(sf::RenderWindow &window)
{
    display<Hover>(window);
}

void UI::Button::setString(const std::string &text_)
{
    text.setString(text_);
    sf::FloatRect rect = text.getLocalBounds();
    text.setOrigin(rect.left + rect.width / 2.0f, rect.top + rect.height / 2.0f);
}

void UI::Button::draw(sf::RenderWindow &window)
{
    const auto mouse_pos = sf::Mouse::getPosition(window);

    const auto hover = layout.getRect().contains(mouse_pos.x, mouse_pos.y);
    const auto currently_clicked = sf::Mouse::isButtonPressed(sf::Mouse::Button::Left);
    const auto clicked = currently_clicked && !was_clicked_last_frame;
    was_clicked_last_frame = currently_clicked;
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