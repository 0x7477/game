#include <player.hpp>
#include <scenes/game.hpp>
#include <network/rpc.hpp>
#include <cmath>
#include "resource_loader.hpp"

Player::Player(GameObject *game_object, const std::string &name)
    : Script{game_object}, name{name}, synced{game_object->getComponent<Synced>()}, hexagon{image_resources.get("hexagon.png")}
{
}

void Player::updatePosition(const Transform &new_transform)
{
    if (received_messages_count > 0)
        received_messages[1] = received_messages[0];

    received_messages[0] = {new_transform, std::chrono::system_clock::now()};

    received_messages_count = std::max(received_messages_count, received_messages_count + 1);
    last_transform_on_message_arrival = transform;
}

void Player::control(const double &delta_time)
{
    const float speed = 100;
    const auto update_rate_s = 1.0 / 10;

    if (WindowManager::keys[sf::Keyboard::W])
        transform.y += -speed * delta_time;
    if (WindowManager::keys[sf::Keyboard::S])
        transform.y += speed * delta_time;
    if (WindowManager::keys[sf::Keyboard::A])
        transform.x += -speed * delta_time;
    if (WindowManager::keys[sf::Keyboard::D])
        transform.x += speed * delta_time;

    static float time_since_last_update = 0;
    time_since_last_update += delta_time;

    if (time_since_last_update > update_rate_s)
    {
        time_since_last_update = std::fmod(delta_time, update_rate_s);
        synced.executeAt(update_position_rpc, network::RPCTargets::Clients, transform);
    }
}

void Player::interpolateReceivedMovement()
{
    if (received_messages_count == 0)
        return;
    if (received_messages_count == 1)
    {
        transform = std::get<Transform>(received_messages[0]);
        return;
    }

    const auto current_message_time = std::get<std::chrono::time_point<std::chrono::system_clock>>(received_messages[0]);
    const auto last_message_time = std::get<std::chrono::time_point<std::chrono::system_clock>>(received_messages[1]);

    const float time_delta_between_messages = std::chrono::duration<float>(current_message_time - last_message_time).count();
    const float time_since_last_message = std::chrono::duration<float>(std::chrono::system_clock::now() - current_message_time).count();

    transform = last_transform_on_message_arrival.moveTowardsNextPrediction(std::get<Transform>(received_messages[1]), std::get<Transform>(received_messages[0]), time_delta_between_messages, time_since_last_message);
}

void Player::update(const double &delta_time)
{
    if (synced.isOwner())
        control(delta_time);
    else
        interpolateReceivedMovement();
}

void Player::display(sf::RenderWindow &window)
{
    const auto size = hexagon.getSize();
    const auto width{20.0}, height{20.0};
    sf::Sprite sprite;
    sprite.setScale(width / size.x , height / size.y);
    sprite.setTexture(hexagon);

    for (int x = 0; x < 5; x++)
    {
        for (int y = 0; y < 5; y++)
        {
            sprite.setPosition(transform.x + x*width  + 0.5*(y%2) * height,  transform.y+ (0.75)* y *height);
            window.draw(sprite);
        }
    }
}
