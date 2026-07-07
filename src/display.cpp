#include <iostream>

#include "../include/display.h"
#include "../include/cube.h"
#include "../include/constants.h"
#include "../include/cubepices.h"
using namespace std;

int CornerSticker(const Corner& c, int sticker){
        return CornerFaces[c.pieceID][c.axis[sticker]];
    }

int EdgeSticker(const Edge c, int sticker){
    return EdgeFaces[c.pieceID][c.axis[sticker]];
}

void Cube::display(){
    // Top
    cout << "           " << Colours[CornerSticker(corners[0], 0)] << " " << Colours[EdgeSticker(edges[0], 0)] << " " << Colours[CornerSticker(corners[1], 0)] << endl;
    cout << "           " << Colours[EdgeSticker(edges[1], 0)] << " " << Colours[0] << " " << Colours[EdgeSticker(edges[2], 0)] << endl;
    cout << "           " << Colours[CornerSticker(corners[2], 0)] << " " << Colours[EdgeSticker(edges[3], 0)] << " " << Colours[CornerSticker(corners[3], 0)] << endl;
    cout << endl;

    // Middle row
    cout << Colours[CornerSticker(corners[0], 1)] << " " << Colours[EdgeSticker(edges[1], 1)] << " " << Colours[CornerSticker(corners[2], 1)] << "   ";
    cout << Colours[CornerSticker(corners[2], 2)] << " " << Colours[EdgeSticker(edges[3], 1)] << " " << Colours[CornerSticker(corners[3], 2)] << "   ";
    cout << Colours[CornerSticker(corners[3], 1)] << " " << Colours[EdgeSticker(edges[2], 1)] << " " << Colours[CornerSticker(corners[1], 1)] << "   ";
    cout << Colours[CornerSticker(corners[1], 2)] << " " << Colours[EdgeSticker(edges[0], 1)] << " " << Colours[CornerSticker(corners[0], 2)] << endl;

    cout << Colours[EdgeSticker(edges[4], 0)] << " " << Colours[1] << " " << Colours[EdgeSticker(edges[6], 0)] << "   ";
    cout << Colours[EdgeSticker(edges[6], 1)] << " " << Colours[2] << " " << Colours[EdgeSticker(edges[7], 1)] << "   ";
    cout << Colours[EdgeSticker(edges[7], 0)] << " " << Colours[3] << " " << Colours[EdgeSticker(edges[5], 0)] << "   ";
    cout << Colours[EdgeSticker(edges[5], 1)] << " " << Colours[4] << " " << Colours[EdgeSticker(edges[4], 1)] << endl;

    cout << Colours[CornerSticker(corners[4], 1)] << " " << Colours[EdgeSticker(edges[9], 1)] << " " << Colours[CornerSticker(corners[6], 1)] << "   ";
    cout << Colours[CornerSticker(corners[6], 2)] << " " << Colours[EdgeSticker(edges[11], 1)] << " " << Colours[CornerSticker(corners[7], 2)] << "   ";
    cout << Colours[CornerSticker(corners[7], 1)] << " " << Colours[EdgeSticker(edges[10], 1)] << " " << Colours[CornerSticker(corners[5], 1)] << "   ";
    cout << Colours[CornerSticker(corners[5], 2)] << " " << Colours[EdgeSticker(edges[8], 1)] << " " << Colours[CornerSticker(corners[4], 2)] << endl;
    cout << endl;

    // Back
    cout << "           " << Colours[CornerSticker(corners[6], 0)] << " " << Colours[EdgeSticker(edges[11], 0)] << " " << Colours[CornerSticker(corners[7], 0)] << endl;
    cout << "           " << Colours[EdgeSticker(edges[9], 0)] << " " << Colours[5] << " " << Colours[EdgeSticker(edges[10], 0)] << endl;
    cout << "           " << Colours[CornerSticker(corners[4], 0)] << " " << Colours[EdgeSticker(edges[8], 0)] << " " << Colours[CornerSticker(corners[5], 0)] << endl;
    cout << endl;
}

void Cube::displayDebug(){
    auto C = [&](int pos, int sticker) -> string{
        int s = corners[pos].axis[sticker] + 1;
        return "c" + to_string(corners[pos].pieceID + 1) + "-" + to_string(s);
    };

    auto E = [&](int pos, int sticker) -> string{
        int s = edges[pos].axis[sticker] + 1;
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
