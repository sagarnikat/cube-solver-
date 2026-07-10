#pragma once
#include <string>
#include <vector>
#include <chrono>

#include "cube.h"
#include "tables.h"

// Kociemba two-phase solver.
//
//   Phase 1: reduce to G1 = ⟨U,D,L2,R2,F2,B2⟩ using all 18 moves.
//   Phase 2: solve within G1 using the 10 phase-2 moves.
//
// solve() returns a first solution quickly, then keeps searching *longer*
// phase-1 sequences that admit *shorter* phase-2 finishes, keeping the best
// total, until the time budget elapses or phase-1 length alone can't beat it.
class Solver
{
public:
    Cube scramble;
    long long NodesCount = 0;

    Solver(Cube cube);

    // Returns the solution as a space-separated move string (e.g. "R U2 F' ...").
    // budgetMs bounds the refinement phase; the first solution is found long before it.
    std::string solve(double budgetMs = 800.0);

private:
    const Tables* T = nullptr;

    int best;                     // best total move count found so far
    std::vector<int> bestSol;     // solution as Moves[] indices
    std::vector<int> path1;       // current phase-1 path (Moves[] indices)

    std::chrono::steady_clock::time_point deadline;
    bool timeUp = false;
    unsigned checkCtr = 0;

    int h1(int tw, int fl, int sl) const;   // phase-1 lower bound
    int h2(int cp, int ud, int sp) const;   // phase-2 lower bound

    void phase1(int tw, int fl, int sl, int g, int bound, int prevFace);
    void tryPhase2();
    bool phase2(int cp, int ud, int sp, int g, int bound, int prevFace, std::vector<int>& out);

    void tick();                  // throttled time-budget check
};
