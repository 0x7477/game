#include <game/player.hpp>
#include <scenes/game.hpp>
#include <network/rpc.hpp>
#include <cmath>
#include <helper/resource_loader.hpp>

Player::Player(GameObject *game_object, const std::string &name)
    : Script{game_object}, synced{game_object->getComponent<Synced>()}
{
    std::cout << "hi\n";
}

void Player::control(const double &delta_time)
{
}

void Player::update(const float& delta_time)
{

}

void Player::display(sf::RenderWindow &window)
{

}
