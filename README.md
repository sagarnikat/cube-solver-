# Rubik's Cube Solver

A Rubik's Cube solver in C++ using **Kociemba's two-phase algorithm**. Solves any scrambled cube typically in **20–23 moves** within under a second.

## Quick Start

```bash
g++ -std=c++20 -O2 -I. main.cpp src/*.cpp -o cube_solver
./cube_solver
```

## Architecture

### Cubie Model

The cube state is stored as a **cubie model** — 8 corner pieces and 12 edge pieces, each holding a `{pieceID, orientation}` pair:

```
Corner  → { pieceID: 0..7,  orientation: 0..2 }
Edge    → { pieceID: 0..11, orientation: 0..1 }
```

| File | Contents |
|---|---|
| `include/cubepices.h` | `Corner` and `Edge` struct definitions |
| `include/constants.h` | Face colours, move strings, `CornerFaces[8][3]`, `EdgeFaces[12][2]` sticker tables |
| `include/cube.h` / `src/cube.cpp` | `Cube` class: `corners[8]`, `edges[12]`, moves, display, solver hooks |

### Coordinate Convention

| Face | ID | Colour | Axis |
|---|---|---|---|
| U (up) | 0 | White | +y |
| L (left) | 1 | Green | -x |
| F (front) | 2 | Red | +z |
| R (right) | 3 | Blue | +x |
| B (back) | 4 | Orange | -z |
| D (down) | 5 | Yellow | -y |

### Move System

**`src/move.cpp`** — the 6 face moves (`R`, `L`, `U`, `D`, `F`, `B`), each implemented as:
1. A 4-cycle of corner cubies and a 4-cycle of edge cubies
2. Orientation updates for affected pieces

Verified correct and marked **DO NOT CHANGE**. Compound moves (`R2`, `R'`, etc.) are handled by `applyAlgorithm()` calling the primitive move multiple times.

### Display System

**`src/display.cpp`** — renders the cube as an unfolded 2D net:

- `display()` — colour-coded net using emoji squares
- `displayDebug()` — shows piece IDs and orientations
- `displayState()` — verbose corner/edge listing

The net layout is described by a `Facelet[6][3][3]` table that maps each sticker position on the net to a (cubicle, slot) pair, then resolves colour via `CornerFaces` / `EdgeFaces`.

### Solver: Kociemba Two-Phase Algorithm

**`src/solver.cpp`** + helper tables.

#### Phase 1
Reduce the cube to **G₁** = ⟨U, D, L², R², F², B²⟩ using all 18 moves.
Goal state: `twist=0 ∧ flip=0 ∧ slice-edges in slice` (i.e. coordinates all zero).

#### Phase 2
Solve within G₁ using only the 10 phase-2 moves: U, U², U', D, D², D', L², R², F², B².

#### Coordinates

| Coordinate | Range | Meaning |
|---|---|---|
| `twist` | 0..2186 (3⁷) | Corner orientations (8th derived by mod-3 parity) |
| `flip` | 0..2047 (2¹¹) | Edge orientations (12th derived by parity) |
| `slice` | 0..494 (C(12,4)) | Position combination of the 4 E-slice edges |
| `cornPerm` | 0..40319 (8!) | Corner permutation |
| `udEdgePerm` | 0..40319 (8!) | Permutation of the 8 U/D edges |
| `slicePerm` | 0..23 (4!) | Permutation of the 4 slice edges |

**`include/utils.h` / `src/utils.cpp`** — coordinate codecs (get/set) and combinatorics helpers (Lehmer factorial base for permutations, combinatorial number system for combinations).

#### Tables (built once at startup, ~6 MB, ~400 ms)

**`include/tables.h` / `src/tables.cpp`** — precomputed lookup tables:

- **Move tables**: for every coordinate value and every move, the resulting coordinate. Built by calling the verified `move.cpp` on representative cubes — correctness by construction.
- **Pruning tables**: admissible lower bounds via BFS backward from the solved state. Stored as `uint8_t` distance arrays.

| Table | Size |
|---|---|
| `twistMove[2187][18]` | 77 KB |
| `flipMove[2048][18]` | 72 KB |
| `sliceMove[495][18]` | 17 KB |
| `cornMove[40320][10]` | 788 KB |
| `udEdgeMove[40320][10]` | 788 KB |
| `slicePermMove[24][10]` | < 1 KB |
| `pruneTwistSlice[2187×495]` | 1 MB |
| `pruneFlipSlice[2048×495]` | 1 MB |
| `pruneCornSlice[40320×24]` | 945 KB |
| `pruneEdge8Slice[40320×24]` | 945 KB |

#### Search

IDA* with two-phase search:
1. Increase the phase-1 depth bound iteratively
2. At each phase-1 leaf (reached G₁), run a phase-2 IDA* to finish
3. Keep the best solution found
4. Refine until time budget (~800 ms default) expires or phase-1 length alone can't beat the best

Move-sequence pruning: no same-face twice in a row; canonical order for opposite faces (U before D, L before R, F before B).

### Files

```
├── include/
│   ├── cubepices.h      Corner/Edge structs
│   ├── constants.h      Colours, moves, sticker tables
│   ├── cube.h           Cube class declaration
│   ├── display.h        Sticker colour helpers, Facelet/Net
│   ├── input.h          Colour-input helpers (fromStickers)
│   ├── solver.h         Kociemba solver class
│   ├── tables.h         Precomputed lookup tables
│   └── utils.h          Coordinate codecs
├── src/
│   ├── cube.cpp         Cube: reset, applyAlgorithm, solved
│   ├── move.cpp         Face move implementations (verified)
│   ├── display.cpp      Net rendering
│   ├── input.cpp        Sticker-to-cube reconstruction
│   ├── solver.cpp       Two-phase IDA* search
│   ├── tables.cpp       Move/pruning table builder
│   └── utils.cpp        Coordinate codec implementations
├── tools/
│   └── derive.cpp       Reference 3D model (used to generate CornerFaces/Net)
├── main.cpp             Interactive game loop + demo
├── notes.txt            Architecture notes
└── README.md            This file
```

## Interactive Game Loop

The program starts in interactive mode with a menu:

```
╔══════════════════════════════════╗
║   Rubik's Cube Solver v1.0       ║
║   Kociemba Two-Phase Algorithm   ║
╚══════════════════════════════════╝

Menu:
  d     — display the cube
  s N   — scramble with N random moves (default 25)
  m M   — apply a move sequence (e.g. "R U2 F'")
  i     — input cube by colours (face-by-face)
  solve — solve the current cube
  r     — reset to solved state
  q     — quit
```

### Colour Input Mode

When using `i`, enter colours for each face in row-major order (left to right, top to bottom) using single-letter codes:

| Code | Colour     |
|------|------------|
|  `W` | White (U)  |
|  `G` | Green (L)  |
|  `R` | Red (F)    |
|  `B` | Blue (R)   |
|  `O` | Orange (B) |
|  `Y` | Yellow (D) |

The prompt shows the face layout with numbered positions so you know which sticker to enter.

## Performance

Benchmark results from 1000 random scrambles (25 moves each, 100 ms solve budget):

| Metric | Value |
|--------|-------|
| **Pass rate** | **100%** (1000/1000) |
| **Moves** | avg **20.02** | min **9** | max **26** |
| **Time** | avg **105.2 ms** | min **2.8 ms** | max **313.7 ms** |

Build and run the benchmark:

```bash
g++ -std=c++20 -O2 -I. tools/benchmark.cpp src/*.cpp -o tools/benchmark
./tools/benchmark [numScrambles] [budgetMs]
```

## References

- [Kociemba's Two-Phase Algorithm](http://kociemba.org/cube.htm)
