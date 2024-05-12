

#include <iostream>
#include "wave_function_collapse/rule_generator.hpp"
#include <game/map.hpp>
int main(int, char **)
{
        // wfc::RuleGenerator rule_generator{"images/cats.png",3};
        // wfc::RuleGenerator rule_generator{"images/water.png", 3};
        wfc::RuleGenerator rule_generator{"images/chess.png",3};
        // wfc::RuleGenerator rule_generator{"maps/images/close_calls(2).png",3};

        // rule_generator.generateImage(48,48);
        rule_generator.generateImage(3, 3);

        return 0;
    }