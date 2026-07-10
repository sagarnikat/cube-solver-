#include <iostream>
#include <chrono>
#include <random>
#include <string>

#include "include/cube.h"
#include "include/solver.h"
#include "include/constants.h"
using namespace std;
using namespace std::chrono;

static int moveCount(const string& sol){
    if (sol.empty()) return 0;
    int n = 1;
    for (char ch : sol) if (ch == ' ') n++;
    return n;
}

int main(){
    // One-time table build (kept out of per-solve timing).
    auto tb0 = high_resolution_clock::now();
    Tables::get();
    auto tb1 = high_resolution_clock::now();
    cout << "Table build: " << duration_cast<milliseconds>(tb1 - tb0).count() << " ms\n\n";

    mt19937 rng(2025);
    uniform_int_distribution<int> pick(0, 17);

    // --- 1. One worked example -------------------------------------------
    {
        Cube scr;
        string scramble;
        for (int k = 0; k < 25; k++){ int m = pick(rng); scramble += Moves[m]; scramble += ' '; scr.applyAlgorithm(Moves[m]); }
        cout << "Example scramble: " << scramble << "\n";
        Solver solver(scr);
        string sol = solver.solve(500.0);
        cout << "Solution (" << moveCount(sol) << " moves): " << sol << "\n";
        Cube check = scr;
        check.applyAlgorithm(sol);
        cout << "Verified: " << (check.solved() ? "SOLVED ✓" : "NOT SOLVED ✗")
             << "   nodes=" << solver.NodesCount << "\n\n";
    }

    // --- 2. Bulk correctness (fast budget) -------------------------------
    {
        const int N = 10000;
        const double budgetMs = 3.0;    // small budget: near-first solutions, fast
        long long totalLen = 0, maxLen = 0, fails = 0;
        double totalMs = 0;

        for (int i = 0; i < N; i++){
            Cube scr;
            for (int k = 0; k < 25; k++) scr.applyAlgorithm(Moves[pick(rng)]);

            Solver solver(scr);
            auto s0 = high_resolution_clock::now();
            string sol = solver.solve(budgetMs);
            auto s1 = high_resolution_clock::now();
            totalMs += duration_cast<microseconds>(s1 - s0).count() / 1000.0;

            Cube check = scr;
            check.applyAlgorithm(sol);
            if (!check.solved()){ fails++; if (fails <= 3) cout << "  FAIL @" << i << " sol=" << sol << "\n"; }

            int len = moveCount(sol);
            totalLen += len;
            maxLen = max<long long>(maxLen, len);
        }

        cout << "Bulk test (" << N << " scrambles, " << budgetMs << " ms budget):\n";
        cout << "  solved     : " << (N - fails) << "/" << N << (fails ? "  <-- FAILURES" : "  ✓") << "\n";
        cout << "  avg length : " << (double)totalLen / N << " moves\n";
        cout << "  max length : " << maxLen << " moves\n";
        cout << "  avg time   : " << totalMs / N << " ms/solve\n\n";
    }

    // --- 3. Quality demo (full refinement budget) ------------------------
    {
        const int N = 50;
        const double budgetMs = 500.0;
        long long totalLen = 0, maxLen = 0, fails = 0;

        for (int i = 0; i < N; i++){
            Cube scr;
            for (int k = 0; k < 25; k++) scr.applyAlgorithm(Moves[pick(rng)]);
            Solver solver(scr);
            string sol = solver.solve(budgetMs);
            Cube check = scr; check.applyAlgorithm(sol);
            if (!check.solved()) fails++;
            int len = moveCount(sol);
            totalLen += len; maxLen = max<long long>(maxLen, len);
        }
        cout << "Quality demo (" << N << " scrambles, " << budgetMs << " ms budget):\n";
        cout << "  solved     : " << (N - fails) << "/" << N << "\n";
        cout << "  avg length : " << (double)totalLen / N << " moves\n";
        cout << "  max length : " << maxLen << " moves\n";
    }

    return 0;
}
