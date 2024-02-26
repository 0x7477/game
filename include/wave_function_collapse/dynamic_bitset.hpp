#pragma once
#include <cstdint>
#include <vector>
#include <cmath>
#include <iostream>

namespace wfc
{
class DynamicBitset
{
    public:
    DynamicBitset(){}
    DynamicBitset(const unsigned long& bits)
    {
        bytes = std::vector<uint8_t>((std::size_t)((bits + 7) / 8), 0);
    }

    bool get(const unsigned int& bit)
    {
        return bytes[getByte(bit)] & (1 << (bit % 8));
    }

    bool set(const unsigned int& bit)
    {
        const auto& old_value = bytes[getByte(bit)];
        const auto new_value = old_value | 1 << (bit % 8);
        const bool set_bit = old_value != new_value;
        bytes[getByte(bit)] = new_value;

        bits_set += set_bit;

        return set_bit;
    }
    void clear(const unsigned int& bit)
    {
        bytes[getByte(bit)] &= ~(1 << (bit % 8));
    }

    uint8_t getNumberOfHighBits()
    {
        return bits_set;
    }

    std::vector<uint8_t> getEnabledBits()
    {
        std::vector<uint8_t> bits{};

        uint8_t byte_index{0};
        for(auto& byte : bytes)
        {
            uint8_t bit_index{0};
            while(byte > 0)
            {
                if(byte % 2 == 1)
                    bits.push_back(byte_index + bit_index);

                bit_index++;
                byte >>= 1;
            }
            byte_index += 8;
        }
        return bits;
    }

    private:
    unsigned int getByte(const unsigned int& bit)
    {
        return bit/8;
    }
    uint8_t bits_set{0};
    std::vector<uint8_t> bytes;
};
}