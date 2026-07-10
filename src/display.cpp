#include <array>
#include <iostream>
#include <string>

#include "../include/display.h"
#include "../include/cube.h"
#include "../include/constants.h"
#include "../include/cubepices.h"
using namespace std;

// Colour of a given sticker of a corner cubie.
//   slot 0 = U/D-facing sticker, slots 1,2 continue CLOCKWISE (viewed from
//   outside). Orientation is a single cyclic shift of the CornerFaces triple.
// Derived & verified against the 3D reference in tools/derive.cpp.
int CornerSticker(const Corner& c, int sticker){
    return CornerFaces[c.pieceID][(sticker - c.orientation + 3) % 3];
}

// Colour of a given sticker of an edge cubie (slot 0 = U/D or, for E-slice
// edges, L/R sticker; slot 1 = the other). Orientation is a single flip.
int EdgeSticker(const Edge& e, int sticker){
    return EdgeFaces[e.pieceID][(sticker - e.orientation + 2) % 2];
}

namespace {

// One facelet on the unfolded net. kind: 0 = centre, 1 = corner, 2 = edge.
//   centre -> idx is the face-colour id directly.
//   corner -> idx is the corner-cubicle index, slot its sticker slot (0..2).
//   edge   -> idx is the edge-cubicle index,   slot its sticker slot (0..1).
struct Facelet { uint8_t kind, idx, slot; };

// Facelet layout for each face, row-major 3x3. Face order: U, L, F, R, B, D.
// Generated (and proven equivalent to the 3D reference) by tools/derive.cpp.
constexpr Facelet Net[6][3][3] = {
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

// Resolve one facelet to its current sticker-colour id.
int faceletColour(const Cube& cube, const Facelet& f){
    switch (f.kind) {
        case 1:  return CornerSticker(cube.corners[f.idx], f.slot);
        case 2:  return EdgeSticker(cube.edges[f.idx], f.slot);
        default: return f.idx; // centre: idx is the face colour directly
    }
}

} // namespace

void Cube::display(){
    // Colour string for one 3-sticker face row.
    auto row = [&](int face, int r) {
        string s;
        for (int c = 0; c < 3; c++)
            s += string(Colours[faceletColour(*this, Net[face][r][c])]) + ' ';
        return s;
    };

    const string pad(11, ' '); // aligns the U/D faces above F

    // U (top)
    for (int r = 0; r < 3; r++) cout << pad << row(0, r) << '\n';
    cout << '\n';

    // Middle band: L | F | R | B
    for (int r = 0; r < 3; r++)
        cout << row(1, r) << "  " << row(2, r) << "  "
             << row(3, r) << "  " << row(4, r) << '\n';
    cout << '\n';

    // D (bottom)
    for (int r = 0; r < 3; r++) cout << pad << row(5, r) << '\n';
    cout << '\n';
}
void Cube::displayDebug(){
    auto C = [&](int pos, int sticker) -> string {
        int s = (sticker - corners[pos].orientation + 3) % 3 + 1;
        return "c" + to_string(corners[pos].pieceID + 1) + "-" + to_string(s);
    };

    auto E = [&](int pos, int sticker) -> string {
        int s = (sticker - edges[pos].orientation + 2) % 2 + 1;
        return "e" + to_string(edges[pos].pieceID + 1) + "-" + to_string(s);
    };

    // Top
    cout << "                  " << C(0, 0) << "  " << E(0, 0) << "  " << C(1, 0) << '\n';
    cout << "                  " << E(1, 0) << "   G   " << E(2, 0) << '\n';
    cout << "                  " << C(2, 0) << "  " << E(3, 0) << "  " << C(3, 0) << "\n\n";

    // Middle
    cout << C(0, 1) << "  " << E(1, 1) << "  " << C(2, 1) << "    ";
    cout << C(2, 2) << "  " << E(3, 1) << "  " << C(3, 2) << "    ";
    cout << C(3, 1) << "  " << E(2, 1) << "  " << C(1, 1) << "    ";
    cout << C(1, 2) << "  " << E(0, 1) << "  " << C(0, 2) << '\n';

    cout << E(4, 0) << "   R   " << E(6, 0) << "    ";
    cout << E(6, 1) << "   W   " << E(7, 1) << "    ";
    cout << E(7, 0) << "   O   " << E(5, 0) << "    ";
    cout << E(5, 1) << "   Y   " << E(4, 1) << '\n';

    cout << C(4, 1) << "  " << E(9, 1) << "  " << C(6, 1) << "    ";
    cout << C(6, 2) << "  " << E(11, 1) << "  " << C(7, 2) << "    ";
    cout << C(7, 1) << "  " << E(10, 1) << "  " << C(5, 1) << "    ";
    cout << C(5, 2) << "  " << E(8, 1) << "  " << C(4, 2) << "\n\n";

    // Bottom (Back face)
    cout << "                  " << C(6, 0) << "  " << E(11, 0) << "  " << C(7, 0) << '\n';
    cout << "                  " << E(9, 0) << "   B   " << E(10, 0) << '\n';
    cout << "                  " << C(4, 0) << "  " << E(8, 0) << "  " << C(5, 0) << '\n';
}

void Cube::displayState() {

    cout << "------------------------------------------------------------\n";
    cout << "Corners\n";

    for (int i = 0; i < 8; i++) {
        cout << "corner " << i
             << " | pieceID: " << static_cast<int>(corners[i].pieceID)
             << " | orientation: " << static_cast<int>(corners[i].orientation)
             << '\n';
    }

    cout << "\nEdges\n";

    for (int i = 0; i < 12; i++) {
        cout << "edge " << i
             << " | pieceID: " << static_cast<int>(edges[i].pieceID)
             << " | orientation: " << static_cast<int>(edges[i].orientation)
             << '\n';
    }

    cout << "------------------------------------------------------------\n";
}