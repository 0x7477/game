#pragma once
#include "hyperstate.hpp"
#include <algorithm>
#include <ranges>
#include <numeric>
#include <random>
#include <cassert>
#include <iostream>
#include "direction.hpp"
#include <expected>
class HyperState
{
public:
    HyperState(const std::vector<std::array<std::vector<std::size_t>, 4>>& adjacencies, const float &base_entropy, const float &sum)
        : sum{sum}, log_sum{std::log(sum)}, p_log_p_sum{base_entropy}, entropy{log_sum - base_entropy / sum},
          possible_pattern_count(adjacencies.size()),
          hyperstates(possible_pattern_count, true),
          compatibilities(possible_pattern_count)
    {
        for (std::size_t pattern{0u}; pattern < possible_pattern_count; pattern++)
            for (std::size_t direction{0u}; direction < 4; direction++)
                compatibilities[pattern][direction] = adjacencies[pattern][reverse_directions[direction]].size();
    }

    std::size_t collapse(const std::vector<float> &pattern_frequencies, std::minstd_rand &gen)
    {
        std::uniform_real_distribution<> dis(0, sum);

        double random_value = dis(gen);
        size_t selected_index = pattern_frequencies.size() - 1;

        for (std::size_t i{0}; i < pattern_frequencies.size(); i++)
        {
            random_value -= hyperstates[i] ? pattern_frequencies[i] : 0;
            if (random_value > 0)
                continue;

            selected_index = i;
            break;
        }

        selected_pattern = selected_index;

        for (std::size_t c{0u}; c < compatibilities.size(); c++)
        {
            if (c == selected_index)
                continue;
            compatibilities[c] = {0, 0, 0, 0};
        }

        return selected_index;
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


    void updateEntropy(const float &pattern_frequency, const float &p_log_p_pattern_frequency)
    {
        p_log_p_sum -= p_log_p_pattern_frequency;
        sum -= pattern_frequency;
        log_sum = std::log(sum);

        possible_pattern_count--;
        entropy = log_sum - p_log_p_sum / sum;
    }

    float getEntropy()
    {
        return entropy;
    }

    std::expected<bool, bool> isCollapsed()
    {
        if (possible_pattern_count <= 0)
            return std::unexpected(false);
        return possible_pattern_count == 1;
    }
    
    std::size_t getPattern()
    {
        if(selected_pattern)
            return *selected_pattern;

        for (auto i{0u}; i < hyperstates.size(); i++)
        {
            if (!hyperstates[i])
                continue;

            return i;
        }

        assert(false);
        return -1;
    }



    // private:
    float sum, log_sum, p_log_p_sum, entropy;
    std::size_t possible_pattern_count;
    std::optional<std::size_t> selected_pattern{};
    std::vector<bool> hyperstates;
    std::vector<std::array<int, 4>> compatibilities;
};