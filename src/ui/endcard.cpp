#include <ui/endcard.hpp>
UI::EndCard::EndCard()
    : layout{}, result_text{"", font_resources.get("arial.ttf")}
{
    result_text.setFillColor(sf::Color::Black);
}

void UI::EndCard::draw(sf::RenderWindow &window)
{

    if (WindowManager::getKeyDown(sf::Keyboard::Y))
    {
        on_exit();
        return;
    }

    sf::RectangleShape background;

    const auto rect = layout.getRect();
    background.setPosition(rect.x, rect.y);
    background.setSize({rect.width, rect.height});

    window.draw(background);

    result_text.setPosition(layout.getRect().x + layout.getRect().width / 2, layout.getRect().y + layout.getRect().height / 2);
    window.draw(result_text);
}

void UI::EndCard::setResult(const bool &win, std::function<void()> on_exit_)
{
    result_text.setString(win ? "win" : "defeat");
    on_exit = on_exit_;
}
