#include <player.hpp>
#include <network/synced.hpp>
#include <game.hpp>

Player::Player(const std::string &name)
    : name{name} {}

void Player::serialize(std::vector<char> &buffer, const Player &value)
{
    network::serialize_element(buffer, value.health);
    network::serialize_element(buffer, value.name);
}

void Player::deserialize(std::string_view &buffer, Player &value)
{
    network::deserialize_element(buffer, value.health);
    network::deserialize_element(buffer, value.name);
}

void Player::update(sf::RenderWindow& window)
{
    if(Game::keys[sf::Keyboard::W])
        pos_y += -1;
    if(Game::keys[sf::Keyboard::S])
        pos_y += 1;
    if(Game::keys[sf::Keyboard::A])
        pos_x += -1;
    if(Game::keys[sf::Keyboard::D])
        pos_x += 1;

}
void Player::display(sf::RenderWindow& window)
{
    sf::CircleShape circle(10);
    circle.setFillColor(sf::Color::Red);
    circle.setPosition(pos_x,pos_y);
    window.draw(circle);
}
