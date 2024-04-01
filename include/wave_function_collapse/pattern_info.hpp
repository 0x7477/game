#pragma once

class Pattern;

class PatternInfo
{
public:
    PatternInfo(Pattern *pattern,const std::size_t& id, const float &probability, const std::array<std::vector<std::size_t>, 4> &possible_adjacent_patterns)
        : pattern{pattern}, id{id}, probability{probability}, possible_adjacent_patterns{possible_adjacent_patterns} {}

    Pattern *pattern;
    std::size_t id;
    float probability;
    std::array<std::vector<std::size_t>, 4> possible_adjacent_patterns;
};