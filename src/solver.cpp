#include "../include/solver.h"
#include "../include/utils.h"
#include "../include/constants.h"

#include <algorithm>
#include <climits>

using namespace std;
using namespace std::chrono;

// Face id of a move: L=0 R=1 U=2 D=3 F=4 B=5. Axis = face/2 (opposite faces
// share an axis). Used to prune redundant move sequences.
static int faceOf(int moveIdx){ return moveIdx % 6; }

// The face of each phase-2 move ordinal (0..9), same numbering as faceOf.
static const int PHASE2_FACE[Tables::NP2] = {
    2, 2, 2,  // U, U2, U'
    3, 3, 3,  // D, D2, D'
    0, 1, 4, 5 // L2, R2, F2, B2
};

// May a move on face f follow a move on face prev? Forbid repeating a face, and
// for the two faces sharing an axis keep only the canonical order (lower first).
static inline bool allowed(int f, int prev){
    if (prev < 0) return true;
    if (f == prev) return false;
    if (f / 2 == prev / 2 && f < prev) return false;
    return true;
}

Solver::Solver(Cube cube){
    scramble = cube;
}

int Solver::h1(int tw, int fl, int sl) const {
    return max<int>(T->pruneTwistSlice[tw * 495 + sl], T->pruneFlipSlice[fl * 495 + sl]);
}

int Solver::h2(int cp, int ud, int sp) const {
    return max<int>(T->pruneCornSlice[cp * 24 + sp], T->pruneEdge8Slice[ud * 24 + sp]);
}

void Solver::tick(){
    // The budget only limits refinement — never abort before a first solution
    // exists, so solve() always returns a valid answer regardless of budget.
    if (best != INT_MAX && (++checkCtr & 0x3FFF) == 0 && steady_clock::now() > deadline)
        timeUp = true;
}

// Phase-1 IDA*: explore every length-`bound` path whose heuristic stays in
// budget. A node reached at g == bound is necessarily in G1 (h1 == 0), so we
// try to finish it with phase 2.
void Solver::phase1(int tw, int fl, int sl, int g, int bound, int prevFace){
    if (timeUp) return;
    if (g + h1(tw, fl, sl) > bound) return;
    if (g == bound){ tryPhase2(); return; }

    tick();
    for (int m = 0; m < 18; m++){
        int f = faceOf(m);
        if (!allowed(f, prevFace)) continue;
        NodesCount++;
        path1.push_back(m);
        phase1(T->twistMove[tw][m], T->flipMove[fl][m], T->sliceMove[sl][m], g + 1, bound, f);
        path1.pop_back();
        if (timeUp) return;
    }
}

// Complete the current phase-1 solution with the shortest phase-2 sequence that
// still beats `best`. Keeps the improved solution when found.
void Solver::tryPhase2(){
    Cube c = scramble;
    for (int m : path1) c.applyAlgorithm(Moves[m]);

    int cp = getCornPerm(c), ud = getUDEdgePerm(c), sp = getSlicePerm(c);
    int len1 = (int)path1.size();
    int maxP2 = best - len1 - 1;          // need len1 + p2 < best
    if (maxP2 < 0) return;

    int prevFace = len1 ? faceOf(path1.back()) : -1;
    vector<int> out;
    for (int d2 = h2(cp, ud, sp); d2 <= maxP2 && !timeUp; d2++){
        out.clear();
        if (phase2(cp, ud, sp, 0, d2, prevFace, out)){
            best = len1 + d2;
            bestSol = path1;
            bestSol.insert(bestSol.end(), out.begin(), out.end());
            return;
        }
    }
}

// Phase-2 IDA* over the 10 phase-2 moves. Returns the first (hence shortest,
// given increasing `bound`) solution of length exactly `bound`.
bool Solver::phase2(int cp, int ud, int sp, int g, int bound, int prevFace, vector<int>& out){
    if (timeUp) return false;
    if (g + h2(cp, ud, sp) > bound) return false;
    if (g == bound) return cp == 0 && ud == 0 && sp == 0;

    tick();
    for (int j = 0; j < Tables::NP2; j++){
        int f = PHASE2_FACE[j];
        if (!allowed(f, prevFace)) continue;
        NodesCount++;
        out.push_back(Tables::PHASE2_MOVES[j]);
        if (phase2(T->cornMove[cp][j], T->udEdgeMove[ud][j], T->slicePermMove[sp][j], g + 1, bound, f, out))
            return true;
        out.pop_back();
        if (timeUp) return false;
    }
    return false;
}

string Solver::solve(double budgetMs){
    T = &Tables::get();
    NodesCount = 0;
    best = INT_MAX;
    bestSol.clear();
    path1.clear();
    timeUp = false;
    checkCtr = 0;
    deadline = steady_clock::now() + microseconds((long long)(budgetMs * 1000.0));

    int tw = getTwist(scramble), fl = getFlip(scramble), sl = getSlice(scramble);

    // Iterative deepening on the phase-1 bound. Stop once phase-1 length alone
    // can't beat the best total, or the budget runs out.
    for (int bound = h1(tw, fl, sl); bound < best && !timeUp; bound++)
        phase1(tw, fl, sl, 0, bound, -1);

    string s;
    for (size_t i = 0; i < bestSol.size(); i++){
        if (i) s += ' ';
        s += Moves[bestSol[i]];
    }
    return s;
}
