#include "../include/utils.h"

// Pascal's triangle for C(n,k), n<=12 — used by the slice combination codec.
static int binom(int n, int k){
    static int C[13][5];
    static bool built = false;
    if (!built){
        for (int i = 0; i <= 12; i++){
            C[i][0] = 1;
            for (int j = 1; j <= 4; j++)
                C[i][j] = (j > i) ? 0 : C[i-1][j-1] + C[i-1][j];
        }
        built = true;
    }
    return (k < 0 || k > 4 || n < 0 || n > 12) ? 0 : C[n][k];
}

// --- combinatorics -------------------------------------------------------

// Lehmer code in factorial base: index = Σ dᵢ·(n-1-i)!  with dᵢ = #{j>i : perm[i]>perm[j]}.
int permToIndex(const int* perm, int n){
    int index = 0;
    for (int i = 0; i < n; i++){
        index *= (n - i);
        for (int j = i + 1; j < n; j++)
            if (perm[i] > perm[j]) index++;
    }
    return index;
}

void indexToPerm(int index, int n, int* perm){
    int fact = 1;
    for (int i = 2; i < n; i++) fact *= i;   // (n-1)!
    int avail[16];
    for (int i = 0; i < n; i++) avail[i] = i;
    for (int i = 0; i < n; i++){
        int d = index / fact;
        index %= fact;
        perm[i] = avail[d];
        for (int j = d; j < n - i - 1; j++) avail[j] = avail[j + 1];
        if (n - 1 - i > 0) fact /= (n - 1 - i);
    }
}

// Combinatorial number system: for occupied positions c₄>c₃>c₂>c₁ (scanned
// high→low) the index is Σ C(cₖ, k).
int combToIndex(const bool occ[12]){
    int index = 0, k = 4;
    for (int i = 11; i >= 0 && k > 0; i--)
        if (occ[i]) index += binom(i, k--);
    return index;
}

void indexToComb(int index, bool occ[12]){
    for (int i = 0; i < 12; i++) occ[i] = false;
    int k = 4;
    for (int i = 11; i >= 0 && k > 0; i--){
        if (binom(i, k) <= index){ occ[i] = true; index -= binom(i, k); k--; }
    }
}

// --- phase-1 coordinates -------------------------------------------------

int getTwist(const Cube& c){
    int t = 0;
    for (int i = 0; i < 7; i++) t = t * 3 + c.corners[i].orientation;
    return t;
}

void setTwist(Cube& c, int t){
    c.reset();
    int sum = 0;
    for (int i = 6; i >= 0; i--){ int o = t % 3; t /= 3; c.corners[i].orientation = o; sum += o; }
    c.corners[7].orientation = (3 - sum % 3) % 3;   // total twist ≡ 0 (mod 3)
}

int getFlip(const Cube& c){
    int f = 0;
    for (int i = 0; i < 11; i++) f = f * 2 + c.edges[i].orientation;
    return f;
}

void setFlip(Cube& c, int f){
    c.reset();
    int sum = 0;
    for (int i = 10; i >= 0; i--){ int o = f & 1; f >>= 1; c.edges[i].orientation = o; sum += o; }
    c.edges[11].orientation = sum & 1;              // total flip ≡ 0 (mod 2)
}

// E-slice edges are pieces 4,5,6,7; solved slice positions are 4,5,6,7.
static const int SLICE_PIECES[4] = {4, 5, 6, 7};
static const int UD_PIECES[8]    = {0, 1, 2, 3, 8, 9, 10, 11};
static const int SLICE_POS[4]    = {4, 5, 6, 7};
static const int UD_POS[8]       = {0, 1, 2, 3, 8, 9, 10, 11};

int getSlice(const Cube& c){
    bool occ[12] = {false};
    for (int i = 0; i < 12; i++){
        int p = c.edges[i].pieceID;
        if (p >= 4 && p <= 7) occ[i] = true;
    }
    return combToIndex(occ);
}

void setSlice(Cube& c, int s){
    c.reset();
    bool occ[12];
    indexToComb(s, occ);
    int si = 0, ui = 0;
    for (int i = 0; i < 12; i++)
        c.edges[i] = occ[i] ? Edge(SLICE_PIECES[si++], 0) : Edge(UD_PIECES[ui++], 0);
}

// --- phase-2 coordinates -------------------------------------------------

int getCornPerm(const Cube& c){
    int p[8];
    for (int i = 0; i < 8; i++) p[i] = c.corners[i].pieceID;
    return permToIndex(p, 8);
}

void setCornPerm(Cube& c, int idx){
    c.reset();
    int p[8];
    indexToPerm(idx, 8, p);
    for (int i = 0; i < 8; i++) c.corners[i] = Corner(p[i], 0);
}

// Rank a U/D edge piece (0..3, 8..11) into 0..7.
static int udRank(int piece){ return piece < 4 ? piece : piece - 4; }

int getUDEdgePerm(const Cube& c){
    int p[8];
    for (int i = 0; i < 8; i++) p[i] = udRank(c.edges[UD_POS[i]].pieceID);
    return permToIndex(p, 8);
}

void setUDEdgePerm(Cube& c, int idx){
    c.reset();
    int p[8];
    indexToPerm(idx, 8, p);
    for (int i = 0; i < 8; i++) c.edges[UD_POS[i]] = Edge(UD_PIECES[p[i]], 0);
}

int getSlicePerm(const Cube& c){
    int p[4];
    for (int i = 0; i < 4; i++) p[i] = c.edges[SLICE_POS[i]].pieceID - 4;
    return permToIndex(p, 4);
}

void setSlicePerm(Cube& c, int idx){
    c.reset();
    int p[4];
    indexToPerm(idx, 4, p);
    for (int i = 0; i < 4; i++) c.edges[SLICE_POS[i]] = Edge(p[i] + 4, 0);
}
