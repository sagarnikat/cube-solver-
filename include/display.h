#pragma once
#include <cstdint>
#include "cubepices.h"

int CornerSticker(const Corner& c, int sticker);

int EdgeSticker(const Edge& c, int sticker);

// One facelet on the unfolded net. kind: 0 = centre, 1 = corner, 2 = edge.
//   centre -> idx is the face-colour id directly.
//   corner -> idx is the corner-cubicle index, slot its sticker slot (0..2).
//   edge   -> idx is the edge-cubicle index,   slot its sticker slot (0..1).
struct Facelet { uint8_t kind, idx, slot; };

// Facelet layout for each face, row-major 3x3. Face order: U, L, F, R, B, D.
// Generated (and proven equivalent to the 3D reference) by tools/derive.cpp.
inline constexpr Facelet Net[6][3][3] = {
    { // U
        {{1,0,0}, {2,0,0}, {1,1,0}},
        {{2,1,0}, {0,0,0}, {2,2,0}},
        {{1,2,0}, {2,3,0}, {1,3,0}},
    },
    { // L
        {{1,0,2}, {2,1,1}, {1,2,1}},
        {{2,4,0}, {0,1,0}, {2,6,0}},
        {{1,4,1}, {2,9,1}, {1,6,2}},
    },
    { // F
        {{1,2,2}, {2,3,1}, {1,3,1}},
        {{2,6,1}, {0,2,0}, {2,7,1}},
        {{1,6,1}, {2,11,1}, {1,7,2}},
    },
    { // R
        {{1,3,2}, {2,2,1}, {1,1,1}},
        {{2,7,0}, {0,3,0}, {2,5,0}},
        {{1,7,1}, {2,10,1}, {1,5,2}},
    },
    { // B
        {{1,1,2}, {2,0,1}, {1,0,1}},
        {{2,5,1}, {0,4,0}, {2,4,1}},
        {{1,5,1}, {2,8,1}, {1,4,2}},
    },
    { // D
        {{1,6,0}, {2,11,0}, {1,7,0}},
        {{2,9,0}, {0,5,0}, {2,10,0}},
        {{1,4,0}, {2,8,0}, {1,5,0}},
    },
};
