#pragma once

#include "hyperstate.hpp"
#include <span>
#include <queue>
#include <stack>
#include "direction.hpp"
#include <ranges>
#include <concepts>
#include <expected>

enum class ObservationResult
{
    success,
    has_invalid_state
};

class Wave
{

public:
    Wave(const std::size_t &width, const std::size_t &height, const std::size_t &kernel_size,const std::vector<float> &pattern_frequencies, const std::vector<std::array<std::vector<std::size_t>, 4>>& adjacencies, const std::size_t &seed = std::random_device()())
        : width{width}, height{height}, kernel_size{kernel_size},number_of_pattern{pattern_frequencies.size()},
          adjacencies{adjacencies},
          pattern_frequencies{pattern_frequencies},
          p_log_p_pattern_frequencies{getPLogPPatternFrequencies()},
          min_abs_half_plogp{getAbsoluteHalfOfMinimumPLogPFrequency()},
          states(width * height, createInitialHyperState()), gen(seed)
    {
    }

    std::vector<float> getPLogPPatternFrequencies()
    {
        const auto p_log_p_frequencies = pattern_frequencies | std::views::transform([](const float &frequency)
                                                                                     { return frequency * std::log(frequency); });
        return {p_log_p_frequencies.begin(), p_log_p_frequencies.end()};
    }

    float getAbsoluteHalfOfMinimumPLogPFrequency()
    {
        float min_abs_half_plogp = std::numeric_limits<float>::max();

        for (const auto &p_log_p : p_log_p_pattern_frequencies)
            min_abs_half_plogp = std::min(min_abs_half_plogp, (float)std::abs(p_log_p / 2.0));

        return min_abs_half_plogp;
    }

    HyperState createInitialHyperState()
    {
        const auto base_entropy = std::accumulate(p_log_p_pattern_frequencies.begin(), p_log_p_pattern_frequencies.end(), 0.f);
        const auto sum = std::accumulate(pattern_frequencies.begin(), pattern_frequencies.end(), 0.f);

        return HyperState{adjacencies, base_entropy, sum};
    }

    std::tuple<std::size_t, std::size_t> getChordsFromIndex(const std::size_t &index)
    {
        return {index % width, index / width};
    }

    std::size_t getIndexFromChords(const std::tuple<std::size_t, std::size_t> &chords)
    {
        const auto [x, y] = chords;
        return x + y * width;
    }

    std::expected<int, ObservationResult> getIndexOfMinimumEntropy()
    {
        std::uniform_real_distribution<> dis(0, min_abs_half_plogp);

        double min = std::numeric_limits<double>::infinity();
        std::optional<int> minimum_index = {};

        for (std::size_t i{0u}; i < states.size(); i++)
        {
            const auto collapsed = states[i].isCollapsed();

            if (!collapsed.has_value())
                return std::unexpected(ObservationResult::has_invalid_state);
            if (*collapsed)
                continue;

            const auto entropy = states[i].getEntropy();
            if (entropy > min)
                continue;

            const auto noise = dis(gen);

            if (entropy + noise >= min)
                continue;

            min = entropy + noise;
            minimum_index = i;
        }
        if (!minimum_index)
            return std::unexpected(ObservationResult::success);

        return *minimum_index;
    }

    void collapse(const std::size_t index)
    {
        const auto collapsed_pattern = states[index].collapse(pattern_frequencies, gen);

        for (std::size_t pattern{0u}; pattern < number_of_pattern; pattern++)
        {
            if (pattern == collapsed_pattern)
                continue;

            if (!states[index].hyperstates[pattern])
                continue;

            updatedRemovedPattern(index, pattern);
        }
    }

    void updatedRemovedPattern(const std::size_t &index, const std::size_t &pattern)
    {
        propagation_stack.push({index, pattern});
        states[index].updateEntropy(pattern_frequencies[pattern], p_log_p_pattern_frequencies[pattern]);
    }

    bool run()
    {
        for (auto i{0u}; i < width * height; i++)
        {
            const auto index = getIndexOfMinimumEntropy();
            if (!index)
                return index.error() == ObservationResult::success;

            collapse(*index);
            propagate();
        }
        return true;
    }

    void propagate()
    {
        while (propagation_stack.size() > 0)
        {
            const auto [index, pattern] = propagation_stack.top();
            propagation_stack.pop();
            updateStates(index, pattern);
        }
    }

    // void drawProgress()
    // {
    //     drawImpossibleState(-1, -1);
    // }

    // void drawImpossibleState(const std::size_t &collapsed_index, const std::size_t &impossible_index)
    // {
    //     std::vector<std::string> tiles((std::size_t)width * height, " ");

    //     for (unsigned int y = 0; y < height - 2; y++)
    //     {
    //         for (unsigned int x = 0; x < width - 2; x++)
    //             states[x + width * y].drawPatternIntoVector(pattern_info, tiles, x, y, width, kernel_size);
    //     }

    //     for (unsigned int y = 0; y < height; y++)
    //     {
    //         std::cout << "|";
    //         for (unsigned int x = 0; x < width; x++)
    //         {
    //             const auto index = getIndexFromChords({x, y});

    //             if (index == collapsed_index)
    //                 std::cout << "\033[92m" << tiles[getIndexFromChords({x, y})] << "\033[0m ";
    //             else if (index == impossible_index)
    //                 std::cout << "\033[91m" << "X" << "\033[0m ";
    //             else
    //                 std::cout << tiles[getIndexFromChords({x, y})] << " ";
    //         }

    //         std::cout << "|" << std::endl;
    //     }
    // }

    bool AreChordsInBound(const std::tuple<std::size_t, std::size_t> &chords)
    {
        const auto &[x, y] = chords;
        return x < width && y < height; // negative values overflow to high values
    }

    std::vector<std::tuple<std::size_t, std::size_t>> getNeighbors(const std::size_t &index)
    {
        std::vector<std::tuple<std::size_t, std::size_t>> result;
        result.reserve(4);

        const auto [x, y] = getChordsFromIndex(index);

        for (auto direction{0u}; direction < 4; direction++)
        {
            const auto &[dx, dy] = directions[direction];
            const std::tuple new_chords{x + dx, y + dy};

            if (!AreChordsInBound(new_chords))
                continue;

            result.push_back({direction, getIndexFromChords(new_chords)});
        }

        return result;
    }

    void updateStates(const std::size_t &index, const std::size_t &pattern)
    {
        for (const auto &[direction, neighbour_index] : getNeighbors(index))
        {
            for (const auto &pattern_dependency : adjacencies[pattern][direction])
            {
                const auto pattern_no_longer_possible = states[neighbour_index].decrementPatternPossibility(pattern_dependency, direction);

                if (!pattern_no_longer_possible)
                    continue;

                updatedRemovedPattern(neighbour_index, pattern_dependency);
            }
        }
    }

    std::size_t operator[](const std::size_t& x, const std::size_t& y)
    {
        return states[getIndexFromChords({x,y})].getPattern();
    }

    const std::size_t width, height, kernel_size, number_of_pattern;
    // const std::vector<PatternInfo> pattern_info;
    const std::vector<std::array<std::vector<std::size_t>, 4>> adjacencies;
    const std::vector<float> pattern_frequencies{};
    const std::vector<float> p_log_p_pattern_frequencies{};
    const float min_abs_half_plogp;

    std::vector<HyperState> states;
    std::stack<std::tuple<std::size_t, std::size_t>> propagation_stack{};
    std::minstd_rand gen;
};
