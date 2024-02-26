#pragma once
#include <bitset>
#include "rule.hpp"
#include "dynamic_bitset.hpp"

namespace wfc
{
class State
{
    public:
    State(){}
    State(const std::vector<Rule>& rules)
    :possible_rules{rules.size()}
    {}

    bool enable(const unsigned int& rule_index)
    {
        return possible_rules.set(rule_index);
    }

    unsigned int getEntropy()
    {
        return possible_rules.getNumberOfHighBits();
    }

    std::vector<uint8_t> getEnabledRules()
    {
        return possible_rules.getEnabledBits();
    }

    DynamicBitset possible_rules;    
};
}