#pragma once

#include <string>
#include <memory>

namespace network
{
    struct PacketListElement
    {
        PacketListElement(const std::string_view& data)
        :view{data},buffer{data.data()}{}

        operator std::string_view() const
        {
            return view;
        }

        std::string_view view;
        std::shared_ptr<const char> buffer;
    };
    
}