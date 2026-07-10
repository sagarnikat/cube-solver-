#include <iostream>
#include <chrono>
#include <random>
#include <string>
#include <sstream>
#include <algorithm>

#include "include/cube.h"
#include "include/solver.h"
#include "include/constants.h"
#include "include/input.h"
using namespace std;
using namespace std::chrono;

// ---------------------------------------------------------------------------
// demo / self-test
// ---------------------------------------------------------------------------

static int moveCount(const string& sol){
    if (sol.empty()) return 0;
    return 1 + (int)count(sol.begin(), sol.end(), ' ');
}

static void runSelfTest(){
    mt19937 rng(2025);
    uniform_int_distribution<int> pick(0, 17);

    cout << "\n--- Self-test -----------------------------------------------\n";

    {
        Cube scr;
        for (int k = 0; k < 25; k++) scr.applyAlgorithm(Moves[pick(rng)]);
        Solver s(scr);
        string sol = s.solve(500.0);
        Cube check = scr; check.applyAlgorithm(sol);
        cout << "Example: " << moveCount(sol) << " moves — "
             << (check.solved() ? "SOLVED ✓" : "FAIL ✗") << "\n";
    }

    {
        Cube solved;
        Solver s(solved);
        string sol = s.solve(100.0);
        cout << "Solved cube: \"" << sol << "\" — "
             << (sol.empty() ? "OK ✓" : "FAIL ✗") << "\n";
    }

    {
        const int N = 100;
        long long totalLen = 0, maxLen = 0, fails = 0;
        double totalMs = 0;
        for (int i = 0; i < N; i++){
            Cube scr;
            for (int k = 0; k < 25; k++) scr.applyAlgorithm(Moves[pick(rng)]);
            Solver solver(scr);
            auto t0 = high_resolution_clock::now();
            string sol = solver.solve(3.0);
            auto t1 = high_resolution_clock::now();
            totalMs += duration_cast<microseconds>(t1 - t0).count() / 1000.0;
            Cube check = scr; check.applyAlgorithm(sol);
            if (!check.solved()) fails++;
            int len = moveCount(sol);
            totalLen += len; maxLen = max<long long>(maxLen, len);
        }
        cout << "Bulk (" << N << " cubes, first-solution): ";
        cout << (fails ? to_string(N - fails) + "/" + to_string(N) + " ✓" : "all ✓") << "  ";
        cout << "avg " << (double)totalLen/N << "  max " << maxLen
             << "  avg " << totalMs/N << " ms\n";
    }

    {
        const int N = 50;
        long long totalLen = 0, maxLen = 0, fails = 0;
        for (int i = 0; i < N; i++){
            Cube scr;
            for (int k = 0; k < 25; k++) scr.applyAlgorithm(Moves[pick(rng)]);
            Solver solver(scr);
            string sol = solver.solve(500.0);
            Cube check = scr; check.applyAlgorithm(sol);
            if (!check.solved()) fails++;
            int len = moveCount(sol);
            totalLen += len; maxLen = max<long long>(maxLen, len);
        }
        cout << "Quality (" << N << " cubes, 500 ms budget): ";
        cout << (fails ? to_string(N - fails) + "/" + to_string(N) + " ✓" : "all ✓") << "  ";
        cout << "avg " << (double)totalLen/N << "  max " << maxLen << "\n";
    }

    cout << "--- Done ----------------------------------------------------\n\n";
}

// ---------------------------------------------------------------------------
// interactive game loop
// ---------------------------------------------------------------------------

static void showHelp(){
    cout << "\n"
         << "  d            — display the cube\n"
         << "  s [N]        — scramble with N random moves (default 25)\n"
         << "  m MOVES      — apply moves, e.g. \"R U2 F'\"\n"
         << "  i            — input cube by colours (face-by-face)\n"
         << "  solve [budgetMs] — solve the cube (budget ≈ refinement ms)\n"
         << "  r            — reset to solved state\n"
         << "  test         — run self-test suite\n"
         << "  h / help     — this help\n"
         << "  q / quit     — exit\n";
}

int main(){
    auto tb0 = high_resolution_clock::now();
    Tables::get();
    auto tb1 = high_resolution_clock::now();

    cout << "\n"
         << "╔══════════════════════════════════╗\n"
         << "║   Rubik's Cube Solver v1.0       ║\n"
         << "║   Kociemba Two-Phase Algorithm   ║\n"
         << "╚══════════════════════════════════╝\n";
    cout << "Tables built in " << duration_cast<milliseconds>(tb1 - tb0).count() << " ms\n";
    cout << "Type 'h' for help.\n";

    Cube cube;
    mt19937 rng(random_device{}());
    uniform_int_distribution<int> pick(0, 17);
    string line;

    while (true){
        cout << "\n> ";
        if (!getline(cin, line)) break;
        if (line.empty()) continue;

        istringstream iss(line);
        string cmd;
        iss >> cmd;

        if (cmd == "q" || cmd == "quit") break;

        if (cmd == "h" || cmd == "help") { showHelp(); continue; }

        if (cmd == "d" || cmd == "display") { cube.display(); continue; }

        if (cmd == "r" || cmd == "reset") { cube.reset(); cout << "Reset to solved.\n"; continue; }

        if (cmd == "s" || cmd == "scramble"){
            int n = 25;
            string arg; if (iss >> arg) n = stoi(arg);
            for (int k = 0; k < n; k++) cube.applyAlgorithm(Moves[pick(rng)]);
            cube.display();
            cout << "Scrambled with " << n << " moves.\n";
            continue;
        }

        if (cmd == "m" || cmd == "moves"){
            string alg;
            getline(iss, alg);
            size_t pos = alg.find_first_not_of(' ');
            if (pos != string::npos) alg = alg.substr(pos);
            if (!alg.empty()){
                cube.applyAlgorithm(alg);
                cube.display();
                cout << "Applied: " << alg << "\n";
            }
            continue;
        }

        if (cmd == "i" || cmd == "input"){
            if (readStickersInteractively(cube))
                cout << "Cube set from colour input.\n";
            else
                cout << "Input cancelled.\n";
            continue;
        }

        if (cmd == "solve"){
            double budgetMs = 800.0;
            string arg; if (iss >> arg) budgetMs = stod(arg);

            if (cube.solved()){
                cout << "Already solved!\n";
                continue;
            }

            Solver solver(cube);
            auto t0 = high_resolution_clock::now();
            string sol = solver.solve(budgetMs);
            auto t1 = high_resolution_clock::now();
            double ms = duration_cast<microseconds>(t1 - t0).count() / 1000.0;

            cout << "Solution (" << moveCount(sol) << " moves, "
                 << ms << " ms, " << solver.NodesCount << " nodes):\n";
            cout << "  " << sol << "\n";

            Cube check = cube;
            check.applyAlgorithm(sol);
            if (!check.solved()){
                cout << "  WARNING — solution verification FAILED!\n";
            } else {
                cout << "  Apply solution? (y/n): ";
                string ans;
                getline(cin, ans);
                if (!ans.empty() && tolower(ans[0]) == 'y'){
                    cube.applyAlgorithm(sol);
                    cube.display();
                    cout << "  Cube is now solved!\n";
                }
            }
            continue;
        }

        if (cmd == "test"){
            runSelfTest();
            continue;
        }

        cout << "Unknown command '" << cmd << "'. Type 'h' for help.\n";
    }

    cout << "Bye!\n";
    return 0;
}
