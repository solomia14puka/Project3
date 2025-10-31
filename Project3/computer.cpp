#include <array>
#include <barrier>
#include <thread>
#include <vector>
#include <syncstream>
#include <iostream>
#include "computer.h"
#include "actions.h"

using namespace std;

constexpr int NT = 5;

struct Label { char ch; int total; };
static constexpr array<Label, 4> L1{ { {'a',6}, {'b',6}, {'c',5}, {'d',6} } };
static constexpr array<Label, 4> L2{ { {'e',4}, {'f',7}, {'g',8}, {'h',6} } };
static constexpr array<Label, 2> L3{ { {'i',6}, {'j',6} } };

struct Range { int from, count; };

template<size_t M>
array<array<Range, NT>, M>
makeRanges(const array<Label, M>& L, const int(&split)[M][NT]) {
    array<array<Range, NT>, M> R{};
    for (size_t k = 0; k < M; ++k) {
        int pref = 1;                      // нумерац≥€ д≥й ≥з 1
        for (int t = 0; t < NT; ++t) {
            R[k][t] = { pref, split[k][t] };
            pref += split[k][t];
        }
    }
    return R;
}

void run_computation() {
    cout << "Calculation start.\n";

    // статичний розклад по потоках 
    const int L1_split[4][NT] = { {2,1,1,1,1}, {1,2,1,1,1}, {1,1,1,1,1}, {1,1,2,1,1} };
    const int L2_split[4][NT] = { {1,1,1,1,0}, {2,2,1,1,1}, {2,2,2,1,1}, {1,1,1,1,2} };
    const int L3_split[2][NT] = { {2,1,1,1,1}, {1,2,1,1,1} };

    const auto R1 = makeRanges(L1, L1_split); 
    const auto R2 = makeRanges(L2, L2_split);
    const auto R3 = makeRanges(L3, L3_split);

    barrier sync(NT, []() noexcept {
        osyncstream(cout) << "=== next step ===\n";
        });

    auto worker = [&](int tid) {
        // ЋейЇр 1: a,b,c,d
        for (int k = 0; k < 4; ++k) {
            auto [from, cnt] = R1[k][tid];
            for (int i = 0; i < cnt; ++i) f(L1[k].ch, from + i);
        }
        sync.arrive_and_wait();

        // ЋейЇр 2: e,f,g,h
        for (int k = 0; k < 4; ++k) {
            auto [from, cnt] = R2[k][tid];
            for (int i = 0; i < cnt; ++i) f(L2[k].ch, from + i);
        }
        sync.arrive_and_wait();

        // ЋейЇр 3: i,j
        for (int k = 0; k < 2; ++k) {
            auto [from, cnt] = R3[k][tid];
            for (int i = 0; i < cnt; ++i) f(L3[k].ch, from + i);
        }
        sync.arrive_and_wait();
        };

    vector<jthread> pool; pool.reserve(NT);
    for (int t = 0; t < NT; ++t) pool.emplace_back(worker, t);

    cout << "calculation completed.\n";
}