#include <string>
#include <iostream>

# include "../include/solver.h"
# include "../include/constants.h"
using namespace std;

Solver::Solver(Cube cube){
    this->cube = cube;
    NodesCount = 1;
}

string Solver::solveR(int depth, string steps){
    NodesCount++;
    cout<<steps<<endl;
    cube.display();

    if (cube.solved())
        return steps;

    if (depth == 0)
        return "";

    for (int i = 0; i < 18; i++)
    {
        Cube backup = cube;

        cube.applyAlgorithm(Moves[i]);

        string result = solveR(depth - 1, steps + Moves[i]);

        cube = backup;

        if (!result.empty())
            return result;
    }

    return "";
}

void Solver::solve(int depth){
    cout<<solveR(depth,"")<<endl;
    cout<<"total nodes visited "<<NodesCount<<endl;

}

