#pragma once
#include <SFML/Graphics.hpp>
#include <optional>
#include <engine/window_manager.hpp>
#include <functional>
#include <iostream>
#include <helper/resource_loader.hpp>
#include <fstream>
#include <algorithm>
#include <helper/delta_timer.hpp>
#include <numeric>
#include <filesystem>
namespace UI
{

    class GIF
    {
        struct AnimationInfo
        {
            AnimationInfo(const std::string &path, const std::string &root_path);

            std::vector<int> getOffsets(const std::string &path);

            std::vector<int> getNumbers(const std::string &path);

            sf::Sprite spritesheet;
            std::vector<int> timings;
            std::vector<int> offsets;
            unsigned number_of_images;
            float total_time;
        };

    public:
        GIF(const std::string &path, const std::string &root_path);

        void setColor(const sf::Color & color);

        const sf::Texture *getTexture();

        void display(sf::RenderWindow &window, const float &x, const float &y, const float &scale);

        DeltaTimer delta_time;
        float current_display_time{0};
        AnimationInfo animation;
    };
}