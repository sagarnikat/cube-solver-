#pragma once
#include "cube.h"

// ---------------------------------------------------------------------------
// Coordinate codecs for the two-phase (Kociemba) solver.
//
// A "coordinate" is a small integer that captures one aspect of the cube state.
// Each get*() reads a coordinate from a Cube; each set*() rebuilds a Cube that
// has that coordinate value with every *other* piece left solved (a valid
// representative — enough to derive move tables from the verified move.cpp).
//
// Phase-1 coordinates (goal = all 0  ⟺  cube is in G1):
//   twist  0..2186  (3^7)  corner orientations
//   flip   0..2047  (2^11) edge orientations
//   slice  0..494   C(12,4) positions of the 4 E-slice edges (pieces 4,5,6,7)
//
// Phase-2 coordinates (goal = all 0  ⟺  solved):
//   cornPerm    0..40319 (8!) corner permutation
//   udEdgePerm  0..40319 (8!) permutation of the 8 U/D edges
//   slicePerm   0..23    (4!) permutation of the 4 E-slice edges
// ---------------------------------------------------------------------------

// Combinatorics helpers (factorial / combinatorial number systems).
int  permToIndex(const int* perm, int n);       // perm of {0..n-1} -> [0, n!)
void indexToPerm(int index, int n, int* perm);  // inverse of permToIndex
int  combToIndex(const bool occ[12]);           // 4-subset of 12 -> [0, 495)
void indexToComb(int index, bool occ[12]);      // inverse of combToIndex

// Phase-1 coordinates.
int  getTwist(const Cube& c);
void setTwist(Cube& c, int t);
int  getFlip(const Cube& c);
void setFlip(Cube& c, int f);
int  getSlice(const Cube& c);
void setSlice(Cube& c, int s);

// Phase-2 coordinates.
int  getCornPerm(const Cube& c);
void setCornPerm(Cube& c, int p);
int  getUDEdgePerm(const Cube& c);
void setUDEdgePerm(Cube& c, int p);
int  getSlicePerm(const Cube& c);
void setSlicePerm(Cube& c, int p);
