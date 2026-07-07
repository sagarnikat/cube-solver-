#pragma once
#include <array>

constexpr std::array<const char*, 6> Colours = {
    "🟩", "🟥", "⬜", "🟧", "🟨", "🟦"
};
// {U/D,L/R,F/B}
const int CornerFaces[8][3] = {
    {0,1,4}, // c1
    {0,3,4}, // c2
    {0,1,2}, // c3
    {0,3,2}, // c4
    {5,1,4}, // c5
    {5,3,4}, // c6
    {5,1,2}, // c7
    {5,3,2}  // c8
};

// U/D > L/R > F/B
const int EdgeFaces[12][2] = {
    {0,4}, // e1
    {0,1}, // e2
    {0,3}, // e3
    {0,2}, // e4
    {1,4}, // e5
    {3,4}, // e6
    {1,2}, // e7
    {3,2}, // e8
    {5,4}, // e9
    {5,1}, // e10
    {5,3}, // e11
    {5,2}  // e12
};