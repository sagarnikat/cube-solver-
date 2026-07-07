#pragma once

struct Corner
{
    int pieceID;
    int axis[3];

    Corner(int id = 0, int a = 0, int b = 1, int c = 2)
        : pieceID(id), axis{a, b, c} {}
};

struct Edge
{
    int pieceID;
    int axis[2];

    Edge(int id = 0, int a = 0, int b = 1)
        : pieceID(id), axis{a, b} {}
};