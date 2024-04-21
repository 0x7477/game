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
            AnimationInfo(const std::string &path, const std::string &root_path)
                : spritesheet{image_resources.get(path + "sprite_sheet.png")},
                  timings{getOffsets(root_path + path + "timing.txt")},
                  offsets{getOffsets(root_path + path + "width.txt")},
                  number_of_images{(unsigned)timings.size()},
                  total_time{(float)std::accumulate(timings.begin(), timings.end(), 0)}
            {
                auto rect = spritesheet.getTextureRect();
                if (number_of_images > 0)
                    rect.width = offsets[0];

                offsets.insert(offsets.begin(), 0);

                spritesheet.setTextureRect(rect);
            }

            std::vector<int> getOffsets(const std::string &path)
            {
                const auto numbers = getNumbers(path);
                std::vector<int> accumulated_numbers(numbers.size());
                std::partial_sum(numbers.begin(), numbers.end(), accumulated_numbers.begin());

                return accumulated_numbers;
            }

            std::vector<int> getNumbers(const std::string &path)
            {
                std::ifstream file(path);
                if (!file.is_open())
                {
                    std::cerr << "Error opening file: " << path << std::endl;
                    return {}; // Return empty vector if file couldn't be opened
                }

                std::vector<int> numbers;
                int number;
                while (file >> number)
                    numbers.push_back(number);

                file.close();
                return numbers;
            }

            sf::Sprite spritesheet;
            std::vector<int> timings;
            std::vector<int> offsets;
            unsigned number_of_images;
            float total_time;
        };

    public:
        GIF(const std::string &path, const std::string &root_path)
            : animation{path, root_path}
        {
        }

        void setColor(const sf::Color & color)
        {
            animation.spritesheet.setColor(color);
        }

        const sf::Texture *getTexture()
        {
            return animation.spritesheet.getTexture();
        }

        void display(sf::RenderWindow &window, const float &x, const float &y, const float &scale)
        {
            if (animation.number_of_images > 1)
            {

                delta_time.update();
                current_display_time += delta_time;
                unsigned current_index{0};
                while (current_display_time * 100 > animation.timings[current_index] && current_index < animation.number_of_images)
                {
                    current_index++;
                }
                if (current_index >= animation.number_of_images)
                {
                    current_index = 0;
                    current_display_time = 0;
                }
                sf::IntRect rect = animation.spritesheet.getTextureRect();
                rect.left = animation.offsets[current_index];
                animation.spritesheet.setTextureRect(rect);
            }

            animation.spritesheet.setPosition(x, y);

            animation.spritesheet.setScale(scale, scale);

            window.draw(animation.spritesheet);
        }

        DeltaTimer delta_time;
        float current_display_time{0};
        AnimationInfo animation;
    };
}