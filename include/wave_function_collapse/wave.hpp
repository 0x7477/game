#pragma once

#include "hyperstate.hpp"
#include <span>
#include "pattern_info.hpp"

class Wave
{
public:
    Wave(const std::size_t &width, const std::size_t &height, const HyperState &init_state, const std::vector<PatternInfo> &pattern_info)
        : width{width}, height{height}, states(width * height, init_state), pattern_info{pattern_info}
    {
        const auto init_entropy = init_state.getEntropy(pattern_info);
        entropies.reserve(states.size());
        for (std::size_t i = 0; i < states.size(); i++)
            entropies.push_back({init_entropy, i});

        std::shuffle(entropies.begin(), entropies.end(), gen);
        uncollapsed_entropies = {entropies.begin(), entropies.end()};

        // std::cout << std::get<std::size_t>(uncollapsed_entropies[0]) << " " << std::get<float>(uncollapsed_entropies[0]) << "\n";
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

    std::size_t collapse()
    {
        const auto [_, index] = uncollapsed_entropies[0];

        std::cout << "collapsed " << index << "\n";
        states[index].collapse(pattern_info);
        uncollapsed_entropies = uncollapsed_entropies.subspan(1);
        return index;
    }

    bool run()
    {
        while (uncollapsed_entropies.size() > 0)
        {
            const auto changed_index = collapse();
            updateStates(changed_index);

            drawProgress();
        }
        return true;
    }

    void drawProgress()
    {
        for (unsigned int y = 0; y < height; y++)
        {
            std::cout << "|";
            for (unsigned int x = 0; x < width; x++)
                std::cout << states[getIndexFromChords({x, y})].display(pattern_info) << " ";

            std::cout << "|" << std::endl;
        }

        // std::getchar();
    }

    void drawImpossibleState(const std::size_t &collapsed_index, const std::size_t &impossible_index)
    {
        for (unsigned int y = 0; y < height; y++)
        {
            std::cout << "|";
            for (unsigned int x = 0; x < width; x++)
            {
                const auto index = getIndexFromChords({x, y});

                if (index == collapsed_index)
                    std::cout << "\033[92m"<< states[index].display(pattern_info) << "\033[0m ";
                else if (index == impossible_index)
                    std::cout << "\033[91m"<< "X" << "\033[0m ";
                else
                    std::cout << states[index].display(pattern_info) << " ";
            }

            std::cout << "|" << std::endl;
        }

        // std::getchar();
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
        const std::vector<std::tuple<int, int>> directions{{0, 1}, {1, 0}, {0, -1}, {-1, 0}};

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

    void updateStates(const std::size_t &collapsed_index)
    {
        const auto collapsed_pattern = states[collapsed_index].getPattern();

        std::cout << "pattern was " << collapsed_pattern << "\n";

        for (const auto &[direction, neighbour_index] : getNeighbors(collapsed_index))
        {

            states[neighbour_index].updatePossibleStates(pattern_info[collapsed_pattern], direction);

            if(neighbour_index == 665)
            {

                drawImpossibleState(collapsed_index, neighbour_index);

                pattern_info[collapsed_pattern].pattern->draw();
                
                states[neighbour_index].drawPossiblePatterns(pattern_info);
                std::getchar();
            }

            const auto new_entropy = states[neighbour_index].getEntropy(pattern_info);

            if (states[neighbour_index].isStateImpossible())
            {
                // drawImpossibleState(collapsed_index, neighbour_index);
                // pattern_info[collapsed_pattern].pattern->draw();

                // std::cout << "FORBIDS\n";
                // constexpr std::array reverse_directions{2, 3, 0, 1};

                // for (const auto &impossible_pattern : pattern_info[collapsed_pattern].impossible_adjacent_patterns[reverse_directions[direction]])
                // {

                //     impossible_pattern->pattern->draw();
                //     std::cout << "\n";
                // }

                assert(false);
            }

            std::ranges::for_each(
                uncollapsed_entropies | std::views::filter([&](const auto &element)
                                                           { return std::get<std::size_t>(element) == neighbour_index; }),
                [&](auto &element)
                {
                    std::get<float>(element) = new_entropy;
                });
        }

        auto comparator = [](const auto &a, const auto &b)
        {
            return std::get<float>(a) < std::get<float>(b);
        };

        std::sort(uncollapsed_entropies.begin(), uncollapsed_entropies.end(), comparator);
    }

    HyperState &operator[](std::size_t x, std::size_t y)
    {
        return states[x + y * width];
    }

    std::size_t width, height;
    std::vector<HyperState> states;
    std::vector<PatternInfo> pattern_info;

    std::vector<std::tuple<float, std::size_t>> entropies;
    std::span<std::tuple<float, std::size_t>> uncollapsed_entropies;
};
