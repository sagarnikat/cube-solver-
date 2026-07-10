#include "../include/tables.h"
#include "../include/utils.h"
#include "../include/cube.h"
#include "../include/constants.h"

#include <algorithm>
#include <vector>
#include <fstream>
using namespace std;

// Phase-2 moves as indices into Moves[18] = {L,R,U,D,F,B,L2,R2,U2,D2,F2,B2,L',R',U',D',F',B'}.
const int Tables::PHASE2_MOVES[Tables::NP2] = {2, 8, 14, 3, 9, 15, 6, 7, 10, 11};

const Tables& Tables::get(){
    static Tables instance;
    return instance;
}

// --- serialisation helpers ------------------------------------------------

// Magic number + version for the cache file.
static const uint32_t CACHE_MAGIC   = 0x43554245;   // "CUBE"
static const uint32_t CACHE_VERSION = 1;

static bool writeRaw(ofstream& f, const void* data, size_t sz){
    return f.write(static_cast<const char*>(data), sz).good();
}

static bool readRaw(ifstream& f, void* data, size_t sz){
    return f.read(static_cast<char*>(data), sz).good();
}

bool Tables::load(const char* path){
    ifstream f(path, ios::binary);
    if (!f) return false;

    uint32_t magic, ver;
    if (!readRaw(f, &magic, 4) || magic != CACHE_MAGIC) return false;
    if (!readRaw(f, &ver, 4) || ver != CACHE_VERSION) return false;

    auto ok = readRaw(f, twistMove,     sizeof(twistMove))
           && readRaw(f, flipMove,      sizeof(flipMove))
           && readRaw(f, sliceMove,     sizeof(sliceMove))
           && readRaw(f, cornMove,      sizeof(cornMove))
           && readRaw(f, udEdgeMove,    sizeof(udEdgeMove))
           && readRaw(f, slicePermMove, sizeof(slicePermMove))
           && readRaw(f, pruneTwistSlice, sizeof(pruneTwistSlice))
           && readRaw(f, pruneFlipSlice,  sizeof(pruneFlipSlice))
           && readRaw(f, pruneCornSlice,  sizeof(pruneCornSlice))
           && readRaw(f, pruneEdge8Slice, sizeof(pruneEdge8Slice))
           && readRaw(f, &solvedSlice, 4);
    if (!ok) return false;

    return true;
}

void Tables::save(const char* path) const{
    ofstream f(path, ios::binary);
    if (!f) return;

    writeRaw(f, &CACHE_MAGIC, 4);
    writeRaw(f, &CACHE_VERSION, 4);
    writeRaw(f, twistMove,     sizeof(twistMove));
    writeRaw(f, flipMove,      sizeof(flipMove));
    writeRaw(f, sliceMove,     sizeof(sliceMove));
    writeRaw(f, cornMove,      sizeof(cornMove));
    writeRaw(f, udEdgeMove,    sizeof(udEdgeMove));
    writeRaw(f, slicePermMove, sizeof(slicePermMove));
    writeRaw(f, pruneTwistSlice, sizeof(pruneTwistSlice));
    writeRaw(f, pruneFlipSlice,  sizeof(pruneFlipSlice));
    writeRaw(f, pruneCornSlice,  sizeof(pruneCornSlice));
    writeRaw(f, pruneEdge8Slice, sizeof(pruneEdge8Slice));
    writeRaw(f, &solvedSlice, 4);
}

// --- construction ---------------------------------------------------------

Tables::Tables(){
    if (load(CACHE_PATH)) return;
    build();
    save(CACHE_PATH);
}

void Tables::build(){
    buildMoveTables();
    buildPruneTables();
}

// Build a coordinate move table: for every value, set a representative cube,
// apply each move via the verified move layer, and read the new coordinate.
void Tables::buildMoveTables(){
    for (int v = 0; v < 2187; v++){
        Cube c; setTwist(c, v);
        for (int m = 0; m < 18; m++){ Cube t = c; t.applyAlgorithm(Moves[m]); twistMove[v][m] = getTwist(t); }
    }
    for (int v = 0; v < 2048; v++){
        Cube c; setFlip(c, v);
        for (int m = 0; m < 18; m++){ Cube t = c; t.applyAlgorithm(Moves[m]); flipMove[v][m] = getFlip(t); }
    }
    for (int v = 0; v < 495; v++){
        Cube c; setSlice(c, v);
        for (int m = 0; m < 18; m++){ Cube t = c; t.applyAlgorithm(Moves[m]); sliceMove[v][m] = getSlice(t); }
    }
    for (int v = 0; v < 40320; v++){
        Cube c; setCornPerm(c, v);
        for (int j = 0; j < NP2; j++){ Cube t = c; t.applyAlgorithm(Moves[PHASE2_MOVES[j]]); cornMove[v][j] = getCornPerm(t); }
    }
    for (int v = 0; v < 40320; v++){
        Cube c; setUDEdgePerm(c, v);
        for (int j = 0; j < NP2; j++){ Cube t = c; t.applyAlgorithm(Moves[PHASE2_MOVES[j]]); udEdgeMove[v][j] = getUDEdgePerm(t); }
    }
    for (int v = 0; v < 24; v++){
        Cube c; setSlicePerm(c, v);
        for (int j = 0; j < NP2; j++){ Cube t = c; t.applyAlgorithm(Moves[PHASE2_MOVES[j]]); slicePermMove[v][j] = getSlicePerm(t); }
    }

    Cube solved;
    solvedSlice = getSlice(solved);   // == 69
}

// BFS from a start (a,b) over a pair of coordinates. The move set is closed
// under inverse, so forward distance from the goal equals distance-to-goal.
template <class TA, class TB>
static void bfsPair(uint8_t* prune, int Bsize,
                    const TA* moveA, int Astride,
                    const TB* moveB, int Bstride,
                    int nmoves, int startA, int startB, long total){
    std::fill(prune, prune + total, 0xFF);
    auto idx = [&](int a, int b){ return (long)a * Bsize + b; };

    std::vector<int> cur, nxt;
    prune[idx(startA, startB)] = 0;
    cur.push_back((int)idx(startA, startB));

    for (uint8_t depth = 0; !cur.empty(); depth++){
        for (int code : cur){
            int a = code / Bsize, b = code % Bsize;
            const TA* ra = moveA + (long)a * Astride;
            const TB* rb = moveB + (long)b * Bstride;
            for (int m = 0; m < nmoves; m++){
                long ni = idx(ra[m], rb[m]);
                if (prune[ni] == 0xFF){ prune[ni] = depth + 1; nxt.push_back((int)ni); }
            }
        }
        cur.swap(nxt);
        nxt.clear();
    }
}

void Tables::buildPruneTables(){
    // Phase 1: goal = (twist 0, flip 0, slice solvedSlice), all 18 moves.
    bfsPair(pruneTwistSlice, 495,
            &twistMove[0][0], 18, &sliceMove[0][0], 18,
            18, 0, solvedSlice, 2187L * 495);
    bfsPair(pruneFlipSlice, 495,
            &flipMove[0][0], 18, &sliceMove[0][0], 18,
            18, 0, solvedSlice, 2048L * 495);

    // Phase 2: goal = (cornPerm 0, udEdgePerm 0, slicePerm 0), 10 moves.
    bfsPair(pruneCornSlice, 24,
            &cornMove[0][0], NP2, &slicePermMove[0][0], NP2,
            NP2, 0, 0, 40320L * 24);
    bfsPair(pruneEdge8Slice, 24,
            &udEdgeMove[0][0], NP2, &slicePermMove[0][0], NP2,
            NP2, 0, 0, 40320L * 24);
}
