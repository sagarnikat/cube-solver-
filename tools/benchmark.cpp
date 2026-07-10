#include <iostream>
#include <iomanip>
#include <chrono>
#include <random>
#include <algorithm>
#include <limits>

#include "include/cube.h"
#include "include/solver.h"
#include "include/constants.h"
using namespace std;
using namespace std::chrono;

static int moveCount(const string& sol){
    if (sol.empty()) return 0;
    return 1 + (int)count(sol.begin(), sol.end(), ' ');
}

int main(int argc, char** argv){
    int N = 1000;
    double budgetMs = 100.0;
    if (argc > 1) N = stoi(argv[1]);
    if (argc > 2) budgetMs = stod(argv[2]);

    auto tb0 = high_resolution_clock::now();
    Tables::get();
    auto tb1 = high_resolution_clock::now();
    cout << "Tables loaded in " << duration_cast<milliseconds>(tb1 - tb0).count() << " ms\n\n";

    mt19937 rng(random_device{}());
    uniform_int_distribution<int> pick(0, 17);

    long long totalMoves = 0, minMoves = numeric_limits<long long>::max(), maxMoves = 0;
    double totalMs = 0, minTime = numeric_limits<double>::max(), maxTime = 0;
    long long fails = 0;

    cout << "Testing " << N << " scrambles (budget " << budgetMs << " ms each)...\n\n";

    for (int i = 0; i < N; i++){
        Cube scr;
        for (int k = 0; k < 25; k++) scr.applyAlgorithm(Moves[pick(rng)]);

        Solver solver(scr);
        auto t0 = high_resolution_clock::now();
        string sol = solver.solve(budgetMs);
        auto t1 = high_resolution_clock::now();

        double elapsed = duration_cast<microseconds>(t1 - t0).count() / 1000.0;

        Cube check = scr;
        check.applyAlgorithm(sol);
        if (!check.solved()){
            fails++;
            if (fails <= 5)
                cout << "  FAIL #" << i << " (scramble " << i << ")\n";
        }

        int len = moveCount(sol);
        totalMoves += len;
        minMoves = min<long long>(minMoves, len);
        maxMoves = max<long long>(maxMoves, len);
        totalMs += elapsed;
        minTime = min(minTime, elapsed);
        maxTime = max(maxTime, elapsed);

        if ((i + 1) % 100 == 0)
            cout << "  " << (i + 1) << "/" << N << " done ("
                 << (100.0 * (i + 1) / N) << "%)\n";
    }

    double avgMoves = (double)totalMoves / N;
    double avgTime = totalMs / N;

    cout << "\n--- Results (" << N << " scrambles) ---\n";
    cout << "Pass rate:    " << (fails ? to_string(N - fails) + "/" + to_string(N) : "100% (" + to_string(N) + "/" + to_string(N) + ")") << "\n";
    cout << "Moves:        avg " << fixed << setprecision(2) << avgMoves
         << "  min " << minMoves << "  max " << maxMoves << "\n";
    cout << "Time (ms):    avg " << fixed << setprecision(3) << avgTime
         << "  min " << minTime << "  max " << maxTime << "\n";

    return fails > 0 ? 1 : 0;
}
