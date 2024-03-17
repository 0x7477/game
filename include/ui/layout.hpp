#pragma once
#include <SFML/Graphics.hpp>
#include <optional>
#include "window_manager.hpp"
#include <functional>
#include <iostream>

namespace UI
{
struct Layout
{
    enum ConstraintType
    {
        Percent,
        Absolute
    };


    struct Constraint
    {

        Constraint(const float& value): type{Absolute}, value{value}{}
        ConstraintType type;
        float value;
    };

    struct Rect
    {
        float x, y, width, height;

        bool contains(const float& pos_x,const float& pos_y) const
        {
            return pos_x >= x && pos_y >= y && pos_x < x + width && pos_y < y + height;
        }
    };

    
    Layout()
    :rect{calculateRect()}
    {}

    // Layout(Layout& layout): reference{&layout}{rect = calculateRect();


    Layout(const Constraint& top, const Constraint& right, const Constraint& bottom, const Constraint& left)
       :top{top}, right{right}, bottom{bottom},left{left},rect{calculateRect()}
       {

       }


    Layout(Layout& layout, const Constraint& top, const Constraint& right, const Constraint& bottom, const Constraint& left): 
        reference{layout}, top{top}, right{right}, bottom{bottom},left{left},rect{calculateRect()}{}
    const Rect& getRect()
    {
        if(window_width != WindowManager::window_width || window_height != WindowManager::window_height)
            rect = calculateRect();

        return rect;
    }

    static Rect getWindowRect()
    {
        return {.x = 0, .y = 0, .width = (float)WindowManager::window_width, .height = (float)WindowManager::window_height};
    }

    Rect calculateRect()
    {
        window_width = WindowManager::window_width;
        window_height = WindowManager::window_height;
        const Rect reference_rect = (reference)? reference->get().getRect(): getWindowRect();

        const auto x = reference_rect.x + ((left.type == Absolute) ? left.value : left.value * reference_rect.width);
        const auto y = reference_rect.y + ((top.type == Absolute) ? top.value : top.value * reference_rect.height);

        const auto width = reference_rect.x + reference_rect.width -x -((right.type == Absolute) ? right.value : right.value * reference_rect.width);
        const auto height = reference_rect.y + reference_rect.height -y - ((bottom.type == Absolute) ? bottom.value : bottom.value * reference_rect.height);

        return {.x = x, .y = y, .width = width, .height = height};

    }

    unsigned int window_width{0}, window_height{0};
    std::optional<std::reference_wrapper<Layout>> reference{};
    Constraint top{0}, right{0}, bottom{0}, left{0};
    Rect rect; 
};
}

UI::Layout::Constraint operator ""_percent(unsigned long long value);
UI::Layout::Constraint operator ""_percent(long double value);