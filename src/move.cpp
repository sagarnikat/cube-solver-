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
    swap(corners[3].axis[0], corners[3].axis[2]);
    swap(corners[7].axis[0], corners[7].axis[2]);
    swap(corners[5].axis[0], corners[5].axis[2]);
    swap(corners[1].axis[0], corners[1].axis[2]);

    // Edge orientations
    swap(edges[7].axis[0], edges[7].axis[1]);
    swap(edges[10].axis[0], edges[10].axis[1]);
    swap(edges[5].axis[0], edges[5].axis[1]);
    swap(edges[2].axis[0], edges[2].axis[1]);
}

void Cube::move_L(){
    rotateCorner(0, 4, 6, 2);
    rotateEdge(4, 9, 6, 1);

    // Corner orientation updates
    swap(corners[2].axis[0], corners[2].axis[2]);
    swap(corners[6].axis[0], corners[6].axis[2]);
    swap(corners[4].axis[0], corners[4].axis[2]);
    swap(corners[0].axis[0], corners[0].axis[2]);

    // Edge orientations
    swap(edges[6].axis[0], edges[6].axis[1]);
    swap(edges[9].axis[0], edges[9].axis[1]);
    swap(edges[4].axis[0], edges[4].axis[1]);
    swap(edges[1].axis[0], edges[1].axis[1]);
}

void Cube::move_U(){
    rotateCorner(0, 2, 3, 1);
    rotateEdge(0, 1, 3, 2);

    // Corner orientation updates
    swap(corners[0].axis[1], corners[0].axis[2]);
    swap(corners[2].axis[1], corners[2].axis[2]);
    swap(corners[3].axis[1], corners[3].axis[2]);
    swap(corners[1].axis[1], corners[1].axis[2]);
}

void Cube::move_D(){
    rotateCorner(6, 4, 5, 7);
    rotateEdge(9, 8, 10, 11);

    // Corner orientation updates
    swap(corners[6].axis[1], corners[6].axis[2]);
    swap(corners[4].axis[1], corners[4].axis[2]);
    swap(corners[5].axis[1], corners[5].axis[2]);
    swap(corners[7].axis[1], corners[7].axis[2]);
}

void Cube::move_F(){
    rotateCorner(2, 6, 7, 3);
    rotateEdge(6, 11, 7, 3);

    // Corner orientation updates
    swap(corners[2].axis[1], corners[2].axis[0]);
    swap(corners[6].axis[1], corners[6].axis[0]);
    swap(corners[7].axis[1], corners[7].axis[0]);
    swap(corners[3].axis[1], corners[3].axis[0]);
}

void Cube::move_B(){
    rotateCorner(1, 5, 4, 0);
    rotateEdge(5, 8, 4, 0);

    // Corner orientation updates
    swap(corners[1].axis[1], corners[1].axis[0]);
    swap(corners[5].axis[1], corners[5].axis[0]);
    swap(corners[4].axis[1], corners[4].axis[0]);
    swap(corners[0].axis[1], corners[0].axis[0]);
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
