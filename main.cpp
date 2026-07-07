#include <iostream>
#include <chrono>

#include "include/cube.h"
#include "include/display.h"
#include "include/solver.h"
using namespace std;
using namespace std::chrono;

int main() {
    Cube cube;

    cube.move_D();
    cube.move_D();
    cube.move_D();

    Solver solver = Solver(cube);

    auto start = high_resolution_clock::now();

    
    cube.display();
    
    solver.solve(1);
    
    cube.display();

    auto end = high_resolution_clock::now();

    auto total = duration_cast<nanoseconds>(end - start).count();

    cout << "Total time: " << total << " ns\n";
}
