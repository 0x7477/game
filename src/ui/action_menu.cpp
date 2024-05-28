#include <ui/action_menu.hpp>

UI::ActionMenu::ActionMenu()
    : action_text{"", font_resources.get("arial.ttf")},background{image_resources.get("misc/menu.png")}
{
    const float x{200};
    const float y{200};
    action_text.setFillColor(sf::Color::Black);
    background.scale(x / (float)background.getTexture()->getSize().x, y / (float)background.getTexture()->getSize().y);

}

void UI::ActionMenu::draw(sf::RenderWindow &window)
{
    if (WindowManager::getKeyDown(sf::Keyboard::X) && can_cancel)
    {
        on_cancel();
        return;
    }

    if (WindowManager::getKeyDown(sf::Keyboard::Y))
    {
        on_value_selected(current_index);
        return;
    }

    if (options.size() == 0)
        return;

    if (WindowManager::getKeyDown(sf::Keyboard::Down))
        current_index = (current_index + 1) % options.size();

    if (WindowManager::getKeyDown(sf::Keyboard::Up))
        current_index = (current_index - 1 + options.size()) % options.size();

    const auto pos_x = WindowManager::window_width - 300;
    const auto pos_y = 100;
    background.setPosition(pos_x, pos_y);
    window.draw(background);

    for (auto i{0u}; i < options.size(); i++)
    {
        action_text.setFillColor(i == current_index ? sf::Color::Red : sf::Color::Black);
        action_text.setPosition(pos_x+20, pos_y + i * 30);
        action_text.setString(options[i]);
        window.draw(action_text);
    }
}

void UI::ActionMenu::clearOptions()
{
    options.clear();
}

void UI::ActionMenu::setOptions(const std::vector<std::string> &new_options, const std::function<void(const std::size_t &)> &on_value_selected_function, const std::optional<std::function<void()>> &on_cancel_function)
{
    can_cancel = on_cancel_function.has_value();
    options = new_options;
    on_value_selected = on_value_selected_function;
    if (can_cancel)
        on_cancel = *on_cancel_function;
    current_index = 0;
}
