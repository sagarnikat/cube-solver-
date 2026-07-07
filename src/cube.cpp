#include <string>
#include <iostream>
#include "../include/cube.h"
#include "../include/cubepices.h"

Cube :: Cube(){
    reset();
}

void Cube::reset(){
    for(int i =0;i<12;i++){
        edges[i] = Edge(i,0,1);
    }
    for(int i =0; i<8;i++){
        corners[i] = Corner(i,0,1,2);
    }
}

void Cube::applyAlgorithm(const std::string &alg){
    int i = 0;
    while (i < alg.size()) {
        // Skip spaces
        if (alg[i] == ' ') {
            i++;
            continue;
        }
        char face = alg[i++];
        Move(face);
        if (i < alg.size()) {
            if (alg[i] == '2') {
                Move(face);
                i++;
            }
            else if (alg[i] == '\'') {
                Move(face);
                Move(face);
                i++;
            }
        }
    }
}

bool Cube::solved() const{
    for(int i =0;i<12;i++){
        if( i != Cube::edges[i].pieceID ||
            Cube::edges[i].axis[0] != 0 || 
            Cube::edges[i].axis[1] != 1 ){
                return false;
        }
    }

    for(int i = 0;i<8;i++){
        if( i != Cube::corners[i].pieceID ||
            Cube::corners[i].axis[0] != 0 || 
            Cube::corners[i].axis[1] != 1 || 
            Cube::corners[i].axis[2] != 2){

                return false;
            }
    }

    return true;
}



