#pragma once
#include <cstdint>

// ---------------------------------------------------------------------------
// Precomputed lookup tables for the two-phase solver, built once at startup.
//
//  * Move tables  — how each coordinate transitions under a move. Derived by
//    applying the *verified* move.cpp functions to representative cubes, so the
//    tables inherit the move layer's correctness.
//  * Pruning tables — admissible lower bounds (min remaining moves) obtained by
//    breadth-first search backward from the solved coordinate tuple.
//
// Coordinate ranges: twist 2187, flip 2048, slice 495 (phase 1);
//                    cornPerm/udEdgePerm 40320, slicePerm 24 (phase 2).
// Sizes total ~6 MB; the object lives in static storage (Tables::get()).
// ---------------------------------------------------------------------------

struct Tables {
    // Lazily built singleton.
    static const Tables& get();

    // The 10 phase-2 moves, as indices into constants.h `Moves[18]`:
    //   U, U2, U', D, D2, D', L2, R2, F2, B2
    static constexpr int NP2 = 10;
    static const int PHASE2_MOVES[NP2];

    // --- move tables ---------------------------------------------------
    uint16_t twistMove[2187][18];
    uint16_t flipMove[2048][18];
    uint16_t sliceMove[495][18];
    uint16_t cornMove[40320][NP2];
    uint16_t udEdgeMove[40320][NP2];
    uint8_t  slicePermMove[24][NP2];

    // --- pruning tables (byte = distance, 0xFF = unseen) ---------------
    uint8_t pruneTwistSlice[2187 * 495];
    uint8_t pruneFlipSlice[2048 * 495];
    uint8_t pruneCornSlice[40320 * 24];
    uint8_t pruneEdge8Slice[40320 * 24];

    int solvedSlice;   // slice coordinate of the solved cube (== 69)

private:
    Tables();
    void buildMoveTables();
    void buildPruneTables();
};
