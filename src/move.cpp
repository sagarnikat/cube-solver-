#include <algorithm>

#include "../include/cubepices.h"
#include "../include/cube.h"
using namespace std;

void Cube::rotateCorner(int a, int b, int c, int d){
    Corner temp = corners[a];

    corners[a] = corners[b];
    corners[b] = corners[c];
    corners[c] = corners[d];
    corners[d] = temp;
}

void Cube::rotateEdge(int a, int b, int c, int d){
    Edge temp = edges[a];

    edges[a] = edges[b];
    edges[b] = edges[c];
    edges[c] = edges[d];
    edges[d] = temp;
}

void Cube::move_R(){
    rotateCorner(3, 7, 5, 1);
    rotateEdge(7, 10, 5, 2);

    // Corner orientation updates
    corners[1].orientation = (corners[1].orientation + 2) % 3;
    corners[3].orientation = (corners[3].orientation + 1) % 3;
    corners[7].orientation = (corners[7].orientation + 2) % 3;
    corners[5].orientation = (corners[5].orientation + 1) % 3;

    // Edge orientation flips (R affects edge orientation)
    edges[7].orientation ^= 1;
    edges[10].orientation ^= 1;
    edges[5].orientation ^= 1;
    edges[2].orientation ^= 1;
}

void Cube::move_L(){
    rotateCorner(0, 4, 6, 2);
    rotateEdge(4, 9, 6, 1);

    // Corner orientation updates
    corners[0].orientation = (corners[0].orientation + 1) % 3;
    corners[4].orientation = (corners[4].orientation + 2) % 3;
    corners[6].orientation = (corners[6].orientation + 1) % 3;
    corners[2].orientation = (corners[2].orientation + 2) % 3;

    // Edge orientation flips (L affects edge orientation)
    edges[4].orientation ^= 1;
    edges[9].orientation ^= 1;
    edges[6].orientation ^= 1;
    edges[1].orientation ^= 1;
}

void Cube::move_U(){
    rotateCorner(0, 2, 3, 1);
    rotateEdge(0, 1, 3, 2);
    // No orientation changes for U (per spec: only R/L change edge orientation,
    // and U/D never change corner orientation in the U/F/R reference frame)
}

void Cube::move_D(){
    rotateCorner(6, 4, 5, 7);
    rotateEdge(9, 8, 10, 11);
    // No orientation changes for D
}

void Cube::move_F()
{
    rotateCorner(2, 6, 7, 3);
    rotateEdge(6, 11, 7, 3);

    corners[2].orientation = (corners[2].orientation + 1) % 3;
    corners[6].orientation = (corners[6].orientation + 2) % 3;
    corners[7].orientation = (corners[7].orientation + 1) % 3;
    corners[3].orientation = (corners[3].orientation + 2) % 3;

    // Edge orientation NOT affected by F (only R/L flip edges)
}

void Cube::move_B()
{
    rotateCorner(1, 5, 4, 0);
    rotateEdge(5, 8, 4, 0);

    corners[1].orientation = (corners[1].orientation + 1) % 3;
    corners[5].orientation = (corners[5].orientation + 2) % 3;
    corners[4].orientation = (corners[4].orientation + 1) % 3;
    corners[0].orientation = (corners[0].orientation + 2) % 3;

    // Edge orientation NOT affected by B (only R/L flip edges)
}

void Cube::Move(char face){
    switch (face)
    {
    case 'R':
        move_R();
        break;
    case 'L':
        move_L();
        break;
    case 'U':
        move_U();
        break;
    case 'D':
        move_D();
        break;
    case 'F':
        move_F();
        break;
    case 'B':
        move_B();
        break;

    default:
        break;
    }
}