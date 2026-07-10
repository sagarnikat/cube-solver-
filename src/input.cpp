#include <iostream>
#include <string>

#include "../include/input.h"
#include "../include/display.h"
#include "../include/constants.h"
#include "../include/cube.h"
#include "../include/cubepices.h"
using namespace std;

// Reconstruct a Cube from 54 user-supplied face colour IDs.
Cube fromStickers(int stickers[6][9]){
    int cornerCols[8][3]; bool cornerSeen[8]{};
    int edgeCols[12][2];  bool edgeSeen[12]{};

    for (int f = 0; f < 6; f++){
        for (int r = 0; r < 3; r++){
            for (int c = 0; c < 3; c++){
                auto& fl = Net[f][r][c];
                int col = stickers[f][r * 3 + c];
                if (fl.kind == 1){
                    cornerCols[fl.idx][fl.slot] = col;
                    cornerSeen[fl.idx] = true;
                } else if (fl.kind == 2){
                    edgeCols[fl.idx][fl.slot] = col;
                    edgeSeen[fl.idx] = true;
                }
            }
        }
    }

    Cube cube;

    for (int i = 0; i < 8; i++){
        if (!cornerSeen[i]) continue;
        bool found = false;
        for (int p = 0; p < 8 && !found; p++){
            for (int o = 0; o < 3; o++){
                bool match = true;
                for (int s = 0; s < 3 && match; s++){
                    int expected = CornerFaces[p][(s - o + 3) % 3];
                    if (expected != cornerCols[i][s]) match = false;
                }
                if (match){
                    cube.corners[i] = Corner(p, o);
                    found = true;
                    break;
                }
            }
        }
    }

    for (int i = 0; i < 12; i++){
        if (!edgeSeen[i]) continue;
        bool found = false;
        for (int p = 0; p < 12 && !found; p++){
            for (int o = 0; o < 2; o++){
                bool match = true;
                for (int s = 0; s < 2 && match; s++){
                    int expected = EdgeFaces[p][(s - o + 2) % 2];
                    if (expected != edgeCols[i][s]) match = false;
                }
                if (match){
                    cube.edges[i] = Edge(p, o);
                    found = true;
                    break;
                }
            }
        }
    }

    return cube;
}

bool readStickersInteractively(Cube& out){
    static const char* FACE_NAMES[6] = {"U (top)", "L (left)", "F (front)", "R (right)", "B (back)", "D (bottom)"};

    int stickers[6][9];
    cout << "\nEnter colours for each face (row by row, left to right).\n";
    cout << "Codes: W=White  G=Green  R=Red  B=Blue  O=Orange  Y=Yellow\n\n";

    for (int f = 0; f < 6; f++){
        cout << "Face " << FACE_NAMES[f] << ":\n";
        for (int r = 0; r < 3; r++){
            cout << "  Row " << (r+1) << ": ";
            string line;
            if (!getline(cin, line)) return false;
            if (line.size() < 3){
                cout << "  Need 3 characters. Try again.\n";
                r--;
                continue;
            }
            for (int c = 0; c < 3; c++){
                int face = charToFace(line[c]);
                if (face < 0){
                    cout << "  Invalid char '" << line[c] << "'. Use W/G/R/B/O/Y.\n";
                    r--;
                    break;
                }
                stickers[f][r * 3 + c] = face;
            }
        }
    }

    // Validate that each face's centre sticker matches its face colour.
    // Centres are fixed and define the face identity.
    for (int f = 0; f < 6; f++){
        if (stickers[f][4] != f){
            cout << "\nError: centre of " << FACE_NAMES[f]
                 << " is " << faceToChar(stickers[f][4])
                 << " but should be " << faceToChar(f) << ".\n";
            return false;
        }
    }

    Cube cube = fromStickers(stickers);
    cube.display();
    cout << "\nIs this correct? (y/n): ";
    string ans;
    getline(cin, ans);
    if (ans.empty() || tolower(ans[0]) != 'y') return false;

    out = cube;
    return true;
}
