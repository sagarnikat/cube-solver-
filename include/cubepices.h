#pragma once
#include <cstdint>


struct Corner
{
    uint8_t pieceID;
    uint8_t orientation;

    Corner(uint8_t id = 0, uint8_t ori = 0)
        : pieceID(id), orientation(ori) {}
};

struct Edge
{
    uint8_t pieceID;
    uint8_t orientation;

    Edge(uint8_t id = 0, uint8_t ori = 0)
        : pieceID(id), orientation(ori) {}
};