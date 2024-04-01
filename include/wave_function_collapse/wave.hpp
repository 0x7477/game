#pragma once

#include "hyperstate.hpp"
#include <span>
#include "pattern_info.hpp"
#include <queue>
#include <stack>
#include "direction.hpp"
#include <ranges>
#include <concepts>

struct EntropyMemoisation
{
    std::vector<double> plogp_sum; // The sum of p'(pattern) * log(p'(pattern)).
    std::vector<double> sum;       // The sum of p'(pattern).
    std::vector<double> log_sum;   // The log of sum.
    std::vector<double> entropy;   // The entropy of the cell.
};

class Wave
{

public:
    Wave(const std::size_t &width, const std::size_t &height, const std::size_t &kernel_size, const std::vector<PatternInfo> &pattern_info, const std::size_t &seed)
        : width{width}, height{height}, kernel_size{kernel_size},
          pattern_info{pattern_info},
          pattern_frequencies{getPatternFrequencies()},
          p_log_p_pattern_frequencies{getPLogPPatternFrequencies()},
          min_abs_half_plogp{getAbsoluteHalfOfMinimumPLogPFrequency()},
          init_state{createInitialHyperState()},
          states(width * height, init_state), gen(seed)
    {

        std::cout << "min_abs_half_plogp " << min_abs_half_plogp << "\n";
    }

    std::vector<float> getPatternFrequencies()
    {
        const auto frequencies = pattern_info | std::views::transform([](const PatternInfo &pattern)
                                                                      { return pattern.probability; });
        return {frequencies.begin(), frequencies.end()};
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

        return HyperState{pattern_info, base_entropy, sum};
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

    void updateEntropy(const std::size_t &index, const std::size_t &pattern)
    {
        std::cout << "set " << index << " pattern " << pattern << " value " << 0 << "\n";

        states[index].updateEntropy(pattern_frequencies[pattern], p_log_p_pattern_frequencies[pattern]);

        std::cout << index << " memoisation.entropy[index] " << states[index].getEntropy() << "\n";
    }

    int get_min_entropy()
    {
        std::uniform_real_distribution<> dis(0, min_abs_half_plogp);

        // The minimum entropy (plus a small noise)
        double min = std::numeric_limits<double>::infinity();
        int argmin = -1;

        for (unsigned i = 0; i < states.size(); i++)
        {

            // If the cell is decided, we do not compute the entropy (which is equal
            // to 0).
            double nb_patterns_local = states[i].possible_pattern_count;
            if (nb_patterns_local == 1)
                continue;

            // Otherwise, we take the memoised entropy.
            double entropy = states[i].getEntropy();
            std::cout << i << " " << entropy << "\n";

            // We first check if the entropy is less than the minimum.
            // This is important to reduce noise computation (which is not
            // negligible).
            if (entropy <= min)
            {

                // Then, we add noise to decide randomly which will be chosen.
                // noise is smaller than the smallest p * log(p), so the minimum entropy
                // will always be chosen.
                double noise = dis(gen);
                std::cout << i << noise << "\n";
                if (entropy + noise < min)
                {
                    min = entropy + noise;
                    argmin = i;
                }
            }
        }

        return argmin;
    }

    void collapse(const std::size_t index)
    {
        std::cout << "collapsed " << index << std::endl;
        states[index].collapse(pattern_info, gen);

        const auto collapsed_pattern = states[index].getPattern();
        std::cout << "to " << collapsed_pattern << "\n";

        for (std::size_t pattern{0u}; pattern < pattern_info.size(); pattern++)
        {
            if (pattern == collapsed_pattern)
                continue;

            if (!states[index].hyperstates[pattern])
                continue;

            std::cout << "pushing to stack " << index << " " << pattern << "\n";
            propagation_stack.push({index, pattern});

            updateEntropy(index, pattern);
        }
    }

    bool run()
    {
        for (auto i{0u}; i < width * height; i++)
        {
            const auto index = get_min_entropy();

            if (index == -1)
            {
                drawProgress();
                return true;
            }
            collapse(index);
            while (propagation_stack.size() > 0)
            {
                const auto [index, pattern] = propagation_stack.top();
                propagation_stack.pop();
                updateStates(index, pattern);
            }
        }
        return true;
    }

    void drawProgress()
    {
        drawImpossibleState(-1, -1);
    }

    void drawImpossibleState(const std::size_t &collapsed_index, const std::size_t &impossible_index)
    {
        std::vector<std::string> tiles((std::size_t)width * height, " ");

        for (unsigned int y = 0; y < height - 2; y++)
        {
            for (unsigned int x = 0; x < width - 2; x++)
                states[x + width * y].drawPatternIntoVector(pattern_info, tiles, x, y, width, kernel_size);
        }

        for (unsigned int y = 0; y < height; y++)
        {
            std::cout << "|";
            for (unsigned int x = 0; x < width; x++)
            {
                const auto index = getIndexFromChords({x, y});

                if (index == collapsed_index)
                    std::cout << "\033[92m" << tiles[getIndexFromChords({x, y})] << "\033[0m ";
                else if (index == impossible_index)
                    std::cout << "\033[91m" << "X" << "\033[0m ";
                else
                    std::cout << tiles[getIndexFromChords({x, y})] << " ";
            }

            std::cout << "|" << std::endl;
        }
    }

    bool AreChordsInBound(const std::tuple<std::size_t, std::size_t> &chords)
    {
        const auto [x, y] = chords;
        return x >= 0 && x < width && y >= 0 && y < height;
    }

    std::vector<std::tuple<std::size_t, std::size_t>> getNeighbors(const std::size_t &index)
    {
        std::vector<std::tuple<std::size_t, std::size_t>> result;
        result.reserve(4);

        const auto [x, y] = getChordsFromIndex(index);

        for (auto direction{0u}; direction < 4; direction++)
        {
            const auto [dx, dy] = directions[direction];
            const int new_x = x + dx;
            const int new_y = y + dy;

            if (!AreChordsInBound({new_x, new_y}))
                continue;

            result.push_back({direction, getIndexFromChords({new_x, new_y})});
        }

        return result;
    }

    void updateStates(const std::size_t &index, const std::size_t &pattern)
    {
        for (const auto &[direction, neighbour_index] : getNeighbors(index))
        {
            for (const auto &pattern_dependency : pattern_info[pattern].possible_adjacent_patterns[direction])
            {
                std::cout << "because of " << index << " " << pattern << " (direction: " << direction << "):\n";
                std::cout << "value is currently  " << states[neighbour_index].compatibilities[pattern_dependency->id][direction] << "\n";
                std::cout << "decrementing " << neighbour_index << " pattern " << pattern_dependency->id << " direction " << direction << "\n";
                const auto pattern_no_longer_possible = states[neighbour_index].decrementPatternPossibility(pattern_dependency->id, direction);
                std::cout << "value is now  " << states[neighbour_index].compatibilities[pattern_dependency->id][direction] << "\n";

                if (!pattern_no_longer_possible)
                    continue;

                std::cout << "pushing to stack " << neighbour_index << " " << pattern_dependency->id << "\n";

                propagation_stack.push({neighbour_index, pattern_dependency->id});
                updateEntropy(neighbour_index, pattern_dependency->id);

                if (states[neighbour_index].isStateImpossible())
                {
                    std::cout << neighbour_index << "\n";
                    assert(false);
                }
            }
        }
    }

    HyperState &operator[](std::size_t x, std::size_t y)
    {
        return states[x + y * width];
    }

    std::size_t width, height, kernel_size;
    std::vector<PatternInfo> pattern_info;
    std::vector<float> pattern_frequencies{};
    std::vector<float> p_log_p_pattern_frequencies{};
    float min_abs_half_plogp;
    HyperState init_state;
    std::vector<HyperState> states;

    std::stack<std::tuple<std::size_t, std::size_t>> propagation_stack{};

    // EntropyMemoisation memoisation;

    std::minstd_rand gen;
};
