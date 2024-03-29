#pragma once
#include "hyperstate.hpp"
#include <algorithm>
#include <ranges>
#include <numeric>
#include <random>
#include "pattern_info.hpp"
#include <cassert>
#include <iostream>
static std::random_device rd;
static std::mt19937 gen(0);

class HyperState
{
public:
    HyperState(const std::size_t &number_of_patterns)
        : hyperstates(number_of_patterns, true)
    {
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

    void collapse(const std::vector<PatternInfo> &pattern_infos)
    {
        assert(!pattern);
        const auto sum = getProbabilitySum(pattern_infos);

        std::uniform_real_distribution<float> dis(0.0, sum);
        double random_value = dis(gen);

        double cumulative_probability = 0.0;
        size_t selected_index = 0;
        for (std::size_t i{0}; i < pattern_infos.size(); i++)
        {
            cumulative_probability += hyperstates[i] ? pattern_infos[i].probability : 0;
            if (random_value <= cumulative_probability)
                break;

            selected_index++;
        }

        pattern = selected_index;
    }

    std::string display(const std::vector<PatternInfo> &pattern_infos)
    {
        if(!pattern) return " ";

        return std::to_string((int)(*pattern_infos[*pattern].pattern)[1,1]);
    }

    std::size_t getPattern() const
    {
        assert(pattern);
        return *pattern;
    }


    void drawPossiblePatterns(const std::vector<PatternInfo> &pattern_infos)
    {
        for(auto i{0u}; i < hyperstates.size(); i++)
        {
            if(!hyperstates[i])
            continue;
            
            pattern_infos[i].pattern->draw();
            std::cout << "\n";
        }
    }


    void updatePossibleStates(const PatternInfo &pattern_info, const std::size_t &direction)
    {
        if (pattern)
            return; // we don't need to update an already collapsed hyperstate
        
        constexpr std::array reverse_directions{2, 3, 0, 1};
        // constexpr std::array reverse_directions{0, 1,2, 3};

        for (const auto &impossible_pattern : pattern_info.impossible_adjacent_patterns[reverse_directions[direction]])
            hyperstates[impossible_pattern->id] = false;
    }

    bool isStateImpossible()
    {
        if(pattern) return false;
        
        for (const auto &hyperstate : hyperstates)
        {
            if (hyperstate)
                return false;
        }
        return true;
    }

private:
    std::optional<std::size_t> pattern{};
    std::vector<bool> hyperstates;
};