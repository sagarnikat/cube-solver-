#pragma once
#include <string>

#include "cube.h"
using namespace std;

class Solver
{
public:
    Cube cube;
    long long NodesCount;

    Solver(Cube cube);

    string solveR(int depth,string steps);

    void solve(int depth);
};