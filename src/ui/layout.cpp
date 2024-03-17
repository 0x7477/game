#include <ui/button.hpp>

UI::Layout::Constraint operator ""_percent(unsigned long long value)
{
    UI::Layout::Constraint constraint{0};

    constraint.type = UI::Layout::ConstraintType::Percent;
    constraint.value = (float)value / 100.0;

    return constraint;
}

UI::Layout::Constraint operator ""_percent(long double value)
{
    UI::Layout::Constraint constraint{0};

    constraint.type = UI::Layout::ConstraintType::Percent;
    constraint.value = (float)value/ 100.0;

    return constraint;
}