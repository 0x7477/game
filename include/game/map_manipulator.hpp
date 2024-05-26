#pragma once
#include <tuple>
#include <optional>
#include <map>

namespace TileInfo
{
    enum Type
    {
        HQ,
        Base,
        Airport,
        Port,
        Lab,
        City,
        Comm
    };

    enum Team
    {
        Red,
        Blue,
        Green,
        Yellow,
        Darkred,
        Grey,
        Purple,
        Brown,
        Orange,
        Jade,
        Black,
        Cobalt,
        Pink,
        Teal,
        Acid,
        White,
        Aqua
    };

    typedef std::tuple<Type, Team> TileInfo;

    std::map<TileInfo, unsigned> ids{
        {{City, Red}, 38},
        {{Base, Red}, 39},
        {{Airport, Red}, 40},
        {{Port, Red}, 41},
        {{HQ, Red}, 42},
        {{City, Blue}, 43},
        {{Base, Blue}, 44},
        {{Airport, Blue}, 45},
        {{Port, Blue}, 46},
        {{HQ, Blue}, 47},
        {{City, Green}, 48},
        {{Base, Green}, 49},
        {{Airport, Green}, 50},
        {{Port, Green}, 51},
        {{HQ, Green}, 52},
        {{City, Yellow}, 53},
        {{Base, Yellow}, 54},
        {{Airport, Yellow}, 55},
        {{Port, Yellow}, 56},
        {{HQ, Yellow}, 57},
        {{City, Darkred}, 81},
        {{Base, Darkred}, 82},
        {{Airport, Darkred}, 83},
        {{Port, Darkred}, 84},
        {{HQ, Darkred}, 85},
        {{City, Grey}, 86},
        {{Base, Grey}, 87},
        {{Airport, Grey}, 88},
        {{Port, Grey}, 89},
        {{HQ, Grey}, 90},
        {{City, Black}, 91},
        {{Base, Black}, 92},
        {{Airport, Black}, 93},
        {{Port, Black}, 94},
        {{HQ, Black}, 95},
        {{City, Brown}, 96},
        {{Base, Brown}, 97},
        {{Airport, Brown}, 98},
        {{Port, Brown}, 99},
        {{HQ, Brown}, 100},
        {{Airport, Orange}, 117},
        {{Base, Orange}, 118},
        {{City, Orange}, 119},
        {{HQ, Orange}, 120},
        {{Port, Orange}, 121},
        {{Airport, Jade}, 122},
        {{Base, Jade}, 123},
        {{City, Jade}, 124},
        {{HQ, Jade}, 125},
        {{Port, Jade}, 126},
        {{Comm, Orange}, 127},
        {{Comm, Black}, 128},
        {{Comm, Blue}, 129},
        {{Comm, Brown}, 130},
        {{Comm, Green}, 131},
        {{Comm, Jade}, 132},
        {{Comm, Red}, 134},
        {{Comm, Darkred}, 135},
        {{Comm, Yellow}, 136},
        {{Comm, Grey}, 137},
        {{Lab, Orange}, 138},
        {{Lab, Black}, 139},
        {{Lab, Blue}, 140},
        {{Lab, Brown}, 141},
        {{Lab, Green}, 142},
        {{Lab, Grey}, 143},
        {{Lab, Jade}, 144},
        {{Lab, Red}, 146},
        {{Lab, Darkred}, 147},
        {{Lab, Yellow}, 148},

        {{Airport, Cobalt}, 149},
        {{Base, Cobalt}, 150},
        {{City, Cobalt}, 151},
        {{Comm, Cobalt}, 152},
        {{HQ, Cobalt}, 153},
        {{Lab, Cobalt}, 154},
        {{Port, Cobalt}, 155},
        {{Airport, Pink}, 156},
        {{Base, Pink}, 157},
        {{City, Pink}, 158},
        {{Comm, Pink}, 159},
        {{HQ, Pink}, 160},
        {{Lab, Pink}, 161},
        {{Port, Pink}, 162},
        {{Airport, Teal}, 163},
        {{Base, Teal}, 164},
        {{City, Teal}, 165},
        {{Comm, Teal}, 166},
        {{HQ, Teal}, 167},
        {{Lab, Teal}, 168},
        {{Port, Teal}, 169},
        {{Airport, Purple}, 179},
        {{Base, Purple}, 171},
        {{City, Purple}, 172},
        {{Comm, Purple}, 173},
        {{HQ, Purple}, 174},
        {{Lab, Purple}, 175},
        {{Port, Purple}, 176},
        {{Airport, Acid}, 181},
        {{Base, Acid}, 182},
        {{City, Acid}, 183},
        {{Comm, Acid}, 184},
        {{HQ, Acid}, 185},
        {{Lab, Acid}, 186},
        {{Port, Acid}, 187},
        {{Airport, White}, 188},
        {{Base, White}, 189},
        {{City, White}, 190},
        {{Comm, White}, 191},
        {{HQ, White}, 192},
        {{Lab, White}, 193},
        {{Port, White}, 194},
        {{Airport, Aqua}, 196},
        {{Base, Aqua}, 197},
        {{City, Aqua}, 198},
        {{Comm, Aqua}, 199},
        {{HQ, Aqua}, 200},
        {{Lab, Aqua}, 201},
        {{Port, Aqua}, 202},
    };
}

class MapManipulator
{
    std::optional<TileInfo::TileInfo> getTileInfo(const unsigned &index)
    {
        for (const auto &[info, number] : TileInfo::ids)
            if (number == index)
                return info;

        return {};
    }

public:
    unsigned getId(const unsigned &id)
    {
        const auto info = getTileInfo(id);
        if (!info)
            return id;

        std::cout << id << " --> ";
        const auto team = std::get<TileInfo::Team>(*info);
        const auto type = std::get<TileInfo::Type>(*info);

        if (!first_team)
            first_team = team;

        unsigned new_id{0};

        if (team == *first_team)
            new_id =  TileInfo::ids[TileInfo::TileInfo{type, TileInfo::Red}];
        else
            new_id = TileInfo::ids[TileInfo::TileInfo{type, TileInfo::Blue}];

        std::cout << new_id << "\n";
        return new_id;
    }

    std::optional<TileInfo::Team> first_team{};
};