#pragma once
#include <array>
#include <cctype>

// Sticker colour of each face-center, indexed by face id.
// {white, green, red, blue, orange, yellow}
//    0      1      2     3      4       5
//    U      L      F     R      B       D
constexpr std::array<const char*, 6> Colours = {
    "⬜", "🟩", "🟥", "🟦", "🟧", "🟨"
};

constexpr std::array<const char*, 18> Moves = {
    "L","R","U","D","F","B",
    "L2","R2","U2","D2","F2","B2",
    "L'","R'","U'","D'","F'","B'"
};

// The three stickers of every corner, listed CLOCKWISE (viewed from outside the
// cube) starting with the U/D-face sticker. Because the order is chirality-
// consistent across all 8 corners, orientation is a single cyclic shift:
//     colour at slot s  =  CornerFaces[piece][(s - orientation + 3) % 3]
// (slot 0 = U/D-facing, then clockwise). See tools/derive.cpp for the proof.
const int CornerFaces[8][3] = {
    {0,4,1}, // c0 ULB : U B L
    {0,3,4}, // c1 URB : U R B
    {0,1,2}, // c2 ULF : U L F
    {0,2,3}, // c3 URF : U F R
    {5,1,4}, // c4 DLB : D L B
    {5,4,3}, // c5 DRB : D B R
    {5,2,1}, // c6 DLF : D F L
    {5,3,2}  // c7 DRF : D R F
};

// The two stickers of every edge. slot 0 is the U/D sticker (or, for the E-slice
// edges e4..e7 which have no U/D sticker, the L/R sticker); slot 1 is the other.
//     colour at slot s  =  EdgeFaces[piece][(s - orientation + 2) % 2]
const int EdgeFaces[12][2] = {
    {0,4}, // e0: U B
    {0,1}, // e1: U L
    {0,3}, // e2: U R
    {0,2}, // e3: U F
    {1,4}, // e4: L B
    {3,4}, // e5: R B
    {1,2}, // e6: L F
    {3,2}, // e7: R F
    {5,4}, // e8: D B
    {5,1}, // e9: D L
    {5,3}, // e10: D R
    {5,2}  // e11: D F
};

// Colour input helpers — map between face IDs (0..5) and single-letter codes.
inline constexpr const char* COLOUR_CHARS = "WGRBOY";

inline int charToFace(char c){
    c = std::toupper(static_cast<unsigned char>(c));
    for (int i = 0; i < 6; i++) if (COLOUR_CHARS[i] == c) return i;
    return -1;
}

inline char faceToChar(int f){ return COLOUR_CHARS[f]; }