#pragma once
#include "cubepices.h"
#include "display.h"
#include <string>


class Cube
{
public:
    Corner corners[8];
    Edge edges[12];

    Cube();

    void reset();

    void display();
    void displayDebug();

    void applyAlgorithm(const std::string& alg);

    bool solved() const;

    void move_R();
    void move_L();
    void move_U();
    void move_D();
    void move_F();
    void move_B();

    void rotateCorner(int a , int b , int c ,int d);
    void rotateEdge(int a , int b ,int c, int d);

    void Move(char face);
};