#pragma once

class Pattern;

class PatternInfo
{
public:
    PatternInfo(Pattern *pattern,const std::size_t& id, const std::size_t &hash, const float &probability, const std::array<std::vector<PatternInfo *>, 4> &possible_adjacent_patterns)
        : pattern{pattern}, id{id}, hash{hash}, probability{probability}, possible_adjacent_patterns{possible_adjacent_patterns} {}

    Pattern *pattern;
    std::size_t id;
    std::size_t hash;
    float probability;
    std::array<std::vector<PatternInfo *>, 4> possible_adjacent_patterns;
};