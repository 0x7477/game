#pragma once

#include "hyperstate.hpp"
#include <span>
#include "pattern_info.hpp"
#include <queue>
#include <stack>
#include "direction.hpp"

struct EntropyMemoisation
{
    std::vector<double> plogp_sum;     // The sum of p'(pattern) * log(p'(pattern)).
    std::vector<double> sum;           // The sum of p'(pattern).
    std::vector<double> log_sum;       // The log of sum.
    std::vector<unsigned> nb_patterns; // The number of patterns present
    std::vector<double> entropy;       // The entropy of the cell.
};

class Wave
{

public:
    Wave(const std::size_t &width, const std::size_t &height, const std::size_t &kernel_size, const std::vector<PatternInfo> &pattern_info, const std::size_t &seed)
        : width{width}, height{height}, kernel_size{kernel_size}, init_state{pattern_info}, states(width * height, init_state), pattern_info{pattern_info}, gen(seed)
    {
        for (const auto &info : pattern_info)
            pattern_frequencies.push_back(info.probability);

        for (const auto &frequency : pattern_frequencies)
            p_log_p_pattern_frequencies.push_back(frequency * log(frequency));

        for (const auto &p_log_p : p_log_p_pattern_frequencies)
            min_abs_half_plogp = std::min(min_abs_half_plogp, (float)std::abs(p_log_p / 2.0));

        std::cout << "min_abs_half_plogp " << min_abs_half_plogp << "\n";

        double base_entropy = 0;
        double base_s = 0;
        for (unsigned i = 0; i < pattern_info.size(); i++)
        {
            base_entropy += p_log_p_pattern_frequencies[i];
            base_s += pattern_frequencies[i];
        }
        double log_base_s = log(base_s);
        double entropy_base = log_base_s - base_entropy / base_s;
        memoisation.plogp_sum = std::vector<double>(width * height, base_entropy);
        memoisation.sum = std::vector<double>(width * height, base_s);
        memoisation.log_sum = std::vector<double>(width * height, log_base_s);
        memoisation.nb_patterns = std::vector<unsigned>(width * height, static_cast<unsigned>(pattern_info.size()));
        memoisation.entropy = std::vector<double>(width * height, entropy_base);
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

        memoisation.plogp_sum[index] -= p_log_p_pattern_frequencies[pattern];
        memoisation.sum[index] -= pattern_frequencies[pattern];
        memoisation.log_sum[index] = log(memoisation.sum[index]);
        memoisation.nb_patterns[index]--;
        memoisation.entropy[index] =
            memoisation.log_sum[index] -
            memoisation.plogp_sum[index] / memoisation.sum[index];

        std::cout << index << " memoisation.entropy[index] " << memoisation.entropy[index] << "\n";
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
            double nb_patterns_local = memoisation.nb_patterns[i];
            if (nb_patterns_local == 1)
                continue;

            // Otherwise, we take the memoised entropy.
            double entropy = memoisation.entropy[i];
            std::cout << i << " " << memoisation.entropy[i] << "\n";

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
    HyperState init_state;
    std::vector<HyperState> states;
    std::vector<PatternInfo> pattern_info;

    std::stack<std::tuple<std::size_t, std::size_t>> propagation_stack{};

    std::vector<float> pattern_frequencies{};
    std::vector<float> p_log_p_pattern_frequencies{};

    EntropyMemoisation memoisation;
    float min_abs_half_plogp =  std::numeric_limits<float>::max();//99999999999999;

    std::minstd_rand gen;
};
