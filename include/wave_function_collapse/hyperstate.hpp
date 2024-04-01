#pragma once
#include "hyperstate.hpp"
#include <algorithm>
#include <ranges>
#include <numeric>
#include <random>
#include "pattern_info.hpp"
#include <cassert>
#include <iostream>
#include "direction.hpp"
static std::random_device rd;
static std::minstd_rand gen(0);

class HyperState
{
public:
    HyperState(const std::vector<PatternInfo> &pattern_info)
        : hyperstates(pattern_info.size(), true),
          compatibilities(pattern_info.size())
    {
        for (std::size_t pattern{0u}; pattern < pattern_info.size(); pattern++)
            for (std::size_t direction{0u}; direction < 4; direction++)
                compatibilities[pattern][direction] = pattern_info[pattern].possible_adjacent_patterns[reverse_directions[direction]].size();
    }

    float getProbabilitySum(const std::vector<PatternInfo> &pattern_infos) const
    {
        float probability_sum = 0;
        for (std::size_t i{0}; i < pattern_infos.size(); i++)
            probability_sum += hyperstates[i] ? pattern_infos[i].probability : 0;

        return probability_sum;
    }

    float getEntropy(const std::vector<PatternInfo> &pattern_infos) const
    {
        if (pattern)
            return 100000;

        const auto normalizer = 1 / getProbabilitySum(pattern_infos);
        float entropy = 0.0;

        for (std::size_t i{0}; i < pattern_infos.size(); i++)
        {
            if (!hyperstates[i])
                continue;

            const auto probability = pattern_infos[i].probability * normalizer;
            entropy -= probability * std::log2(probability);
        }

        return entropy;
    }

    void collapse(const std::vector<PatternInfo> &pattern_infos, std::minstd_rand &gen)
    {
        assert(!pattern);

        const auto sum = getProbabilitySum(pattern_infos);

        std::uniform_real_distribution<> dis(0, sum);

        double random_value = dis(gen);
        std::cout << "random_value " << random_value << "\n";
        size_t selected_index = pattern_infos.size() - 1;

        for (std::size_t i{0}; i < pattern_infos.size(); i++)
        {
            random_value -= hyperstates[i] ? pattern_infos[i].probability : 0;
            if (random_value <= 0)
            {
                selected_index = i;
                break;
            }
        }

        pattern = selected_index;

        for (std::size_t c{0u}; c < compatibilities.size(); c++)
        {
            if (c == selected_index)
                continue;
            compatibilities[c] = {0, 0, 0, 0};
        }
    }

    std::string display(const std::vector<PatternInfo> &pattern_infos)
    {
        if (!pattern)
            return " ";

        return std::to_string((int)(*pattern_infos[*pattern].pattern)[1, 1]);
    }

    std::size_t getPattern() const
    {
        assert(pattern);
        return *pattern;
    }


    bool decrementPatternPossibility(const std::size_t &pattern, const std::size_t &direction)
    {
        compatibilities[pattern][direction]--;

        if (compatibilities[pattern][direction] == 0)
        {
            hyperstates[pattern] = false;
            compatibilities[pattern] = {0, 0, 0, 0};
        }
        return compatibilities[pattern][direction] == 0;
    }

    bool isStateImpossible()
    {
        if (pattern)
            return false;

        for (const auto &hyperstate : hyperstates)
        {
            if (hyperstate)
                return false;
        }
        return true;
    }

    void drawPatternIntoVector(const std::vector<PatternInfo> &pattern_infos, std::vector<std::string> &tiles, const std::size_t &x, const std::size_t &y, const std::size_t &width, const std::size_t &kernel_size)
    {
        if (!pattern)
        {
            for(auto i{0u}; i < hyperstates.size(); i++)
            {
                if(!hyperstates[i]) 
                continue;
                
                assert(!pattern);
                pattern = i;
            }
        }

        for (auto pattern_y{0u}; pattern_y < kernel_size; pattern_y++)
            for (auto pattern_x{0u}; pattern_x < kernel_size; pattern_x++)
            {
                const auto string = std::to_string((int)(*pattern_infos[*pattern].pattern)[pattern_x, pattern_y]);
                auto &place = tiles[(x + pattern_x) + width * (y + pattern_y)];

                if (!(place == " " || place == string))
                {
                    std::cout << "illegal " << place << " in " << (x + pattern_x) << " " << (y + pattern_y) << "\n";
                    assert(place == " " || place == string);
                }
                place = string;
            }
    }

    // private:
    std::optional<std::size_t> pattern{};
    std::vector<bool> hyperstates;
    std::vector<std::array<int, 4>> compatibilities;
};