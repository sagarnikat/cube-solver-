#include <iostream>
#include "include/cube.h"
#include "include/display.h"
using namespace std;

int main(){
    Cube c;
    
    c.display();
    
    c.applyAlgorithm("R U R' U' F2 L D B' U2 L' F R2 D' B U L2 D2 F' R B2");

    c.display();

    c.applyAlgorithm("B2 R' F D2 L2 U' B' D R2 F' L U2 B D' L' F2 U R U' R'");

    c.display();

    return 0;
}