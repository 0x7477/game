

#include <iostream>
#include "wave_function_collapse/rule.hpp"
#include "wave_function_collapse/field.hpp"
#include "wave_function_collapse/direction.hpp"
#include "wave_function_collapse/dynamic_bitset.hpp"
#include "wave_function_collapse/rule_generator.hpp"

int main(int argc, char **argv)
{
    wfc::RuleGenerator rule_generator{"images/cats.png"};
    std::srand(time(NULL));

    // const std::vector rules{
    //     Rule{{White}, {Black}, Direction{-1, -0}},
    //     Rule{{White}, {Black}, Direction{-0, -1}},
    //     Rule{{Black}, {White}, Direction{-1, -0}},
    //     Rule{{Black}, {White}, Direction{-0, -1}},
    //     Rule{{White}, {Black}, Direction{1, 0}},
    //     Rule{{White}, {Black}, Direction{0, 1}},
    //     Rule{{Black}, {White}, Direction{1, 0}},
    //     Rule{{Black}, {White}, Direction{0, 1}}
    //     };

    const int x{8}, y{8};

    wfc::Field<x, y> chess{rule_generator.generateRules(3)};

    for (int x_{0}; x_ < x; x_++)
    {
        for (int y_{0}; y_ < y; y_++)
        {
            std::cout << chess[x_][y_] << " ";
        }
        std::cout << "\n";
    }
    return 0;
}