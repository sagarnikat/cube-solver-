#pragma once
#include "cube.h"

// Reconstruct a Cube from 54 user-supplied face colour IDs.
// stickers[face][r*3+c] = face colour id (0..5) as seen on the Net.
Cube fromStickers(int stickers[6][9]);

// Prompt the user for all 6 faces' stickers interactively.
// Returns true on success (user confirmed), false on EOF or cancellation.
bool readStickersInteractively(Cube& out);
