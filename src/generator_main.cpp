

#include <iostream>
#include "wave_function_collapse/rule_generator.hpp"

int main(int argc, char **argv)
{
    wfc::RuleGenerator rule_generator{"images/water.png",3};
    // wfc::RuleGenerator rule_generator{"images/chess.png",2};

    rule_generator.generateImage(40,40);

    
    return 0;
}