#ifndef HASHMAP_CPP
#define HASHMAP_CPP

#include <random>
#include <benchmark/benchmark.h>
#include <Core/DataStructure/HashMap.hpp>
#include <unordered_map>
#include <unordered_set>
#include <Core/DataStructure/spe/bytell_hash_map.hpp>

using namespace TRE;

template<typename K, typename V>
using StdHashMap =
    std::unordered_map<K, V>;
    // ska::bytell_hash_map<K, V>;


int32 GetRandomInt(int32 start = 0, int32 end = INT32_MAX - 1)
{
    std::random_device rd; // obtain a random number from hardware
    std::mt19937 gen(rd()); // seed the generator
    std::uniform_int_distribution<> dist2(start, end); // define the range
    return dist2(gen);
}

void HashMapEmptyDecl(benchmark::State& state)
{
    for (auto _ : state) {
        HashMap<int, int> map;
        benchmark::DoNotOptimize(map);
    }
}

void StdHashMapEmptyDecl(benchmark::State& state)
{
    for (auto _ : state) {
        StdHashMap<int, int> map;
        benchmark::DoNotOptimize(map);
    }
}

void HashMapInsertion(benchmark::State& state)
{
    HashMap<int, int> map;
    int f = 1, s = 2;

    for (auto _ : state) {
        map.Emplace(s, 2 * s);
        f = s;
        s = f + s;
    }

    benchmark::DoNotOptimize(map);
}

void StdHashMapInsertion(benchmark::State& state)
{
    StdHashMap<int, int> map;
    int f = 1, s = 2;

    for (auto _ : state) {
        map.emplace(s, 2 * s);
        f = s;
        s = f + s;
    }
}

void HashMapSuccessfullLookUp(benchmark::State& state)
{
    const auto NB = state.max_iterations;
    HashMap<int, int> map;
    int f = 1, s = 2;

    for (usize i = 0; i < NB; i++) {
        map.Emplace(s, 2 * s);
        f = s;
        s = f + s;
    }

    f = 1; s = 2;

    for (auto _ : state) {
        auto p = map.Find(s);
        f = s;
        s = f + s;
        benchmark::DoNotOptimize(p);
    }

    benchmark::DoNotOptimize(map);
}

void StdHashMapSuccessfullLookUp(benchmark::State& state)
{
    const auto NB = state.max_iterations;
    StdHashMap<int, int> map;
    int f = 1, s = 2;

    for (usize i = 0; i < NB; i++) {
        map.emplace(s, 2 * s);
        f = s;
        s = f + s;
    }

    f = 1; s = 2;

    for (auto _ : state) {
        auto p = map.find(s);
        f = s;
        s = f + s;
        benchmark::DoNotOptimize(p);
    }

    benchmark::DoNotOptimize(map);
}

void HashMapUnsuccessfullLookUp(benchmark::State& state)
{
    const auto NB = state.max_iterations;
    HashMap<int, int> map;

    for (usize i = 0; i < NB; i++) {
        map.Emplace(GetRandomInt(NB, 3 * NB), i);
    }

    usize i = 0;

    for (auto _ : state) {
        auto p = map.Find(i++);
        benchmark::DoNotOptimize(p);
    }

    benchmark::DoNotOptimize(map);
}

void StdHashMapUnsuccessfullLookUp(benchmark::State& state)
{
    const auto NB = state.max_iterations;
    StdHashMap<int, int> map;

    for (usize i = 0; i < NB; i++) {
        map.emplace(GetRandomInt(NB, 3 * NB), i);
    }

    usize i = 0;

    for (auto _ : state) {
        auto p = map.find(i++);
        benchmark::DoNotOptimize(p);
    }

    benchmark::DoNotOptimize(map);
}

void HashMapErease(benchmark::State& state)
{
    const auto NB = state.max_iterations;
    HashMap<int, int> map;
    int f = 1, s = 2;

    for (usize i = 0; i < NB; i++) {
        map.Emplace(s, 2 * s);
        f = s;
        s = f + s;
    }

    f = 1; s = 2;

    for (auto _ : state) {
        auto p = map.Erase(s);
        f = s;
        s = f + s;
        benchmark::DoNotOptimize(p);
    }

    benchmark::DoNotOptimize(map);
}

void StdHashMapErease(benchmark::State& state)
{
    const auto NB = state.max_iterations;
    StdHashMap<int, int> map;
    int f = 1, s = 2;

    for (usize i = 0; i < NB; i++) {
        map.emplace(s, 2 * s);
        f = s;
        s = f + s;
    }

    f = 1; s = 2;

    for (auto _ : state) {
        auto p = map.erase(s);
        f = s;
        s = f + s;
        benchmark::DoNotOptimize(p);
    }

    benchmark::DoNotOptimize(map);
}

//BENCHMARK(HashMapEmptyDecl);
//BENCHMARK(StdHashMapEmptyDecl);

//BENCHMARK(HashMapInsertion);
//BENCHMARK(StdHashMapInsertion);

//BENCHMARK(HashMapSuccessfullLookUp);
//BENCHMARK(StdHashMapSuccessfullLookUp);

//BENCHMARK(HashMapUnsuccessfullLookUp);
//BENCHMARK(StdHashMapUnsuccessfullLookUp);

//BENCHMARK(HashMapErease);
//BENCHMARK(StdHashMapErease);

/*std::vector<std::pair<int, int>> inserting;
std::vector<std::pair<int, int>> okLookups;
std::vector<std::pair<int, int>> nonOkLookups;
std::vector<std::pair<int, int>> erase;

void HashMapInsertion(benchmark::State& state)
{
    const auto NB = state.max_iterations;
    HashMap<int, int> map;
    for (uint i = 0; i < NB; i++) {
        auto k = GetRandomInt();
        auto v = GetRandomInt();
        inserting.emplace_back(k, v);
    }
    int i = 0;
    for (auto _ : state) {
        state.PauseTiming();
        auto& p = inserting[i++];
        state.ResumeTiming();

        map.Emplace(p.first, p.second);
    }
    benchmark::DoNotOptimize(map);
}

void StdHashMapInsertion(benchmark::State& state)
{
    StdHashMap<int, int> map;
    int i = 0;
    for (auto _ : state) {
        state.PauseTiming();
        auto& p = inserting[i++];
        state.ResumeTiming();

        map.emplace(p.first, p.second);
    }
    benchmark::DoNotOptimize(map);
}

void HashMapSuccessfullLookUp(benchmark::State& state)
{
    const auto NB = state.max_iterations;
    HashMap<int, int> map;
    usize i = 0;
    for (; i < NB; i++) {
        auto k = GetRandomInt();
        auto v = GetRandomInt();
        map.Emplace(k, v);
        okLookups.emplace_back(k, v);
    }
    i = 0;
    for (auto _ : state) {
        state.PauseTiming();
        auto& pair = okLookups[i++];
        state.ResumeTiming();

        auto p = map.Find(pair.first);
        benchmark::DoNotOptimize(p);
    }
    benchmark::DoNotOptimize(map);
}

void StdHashMapSuccessfullLookUp(benchmark::State& state)
{
    const auto NB = state.max_iterations;
    StdHashMap<int, int> map;
    usize i = 0;
    for (; i < NB; i++) {
        auto& pair = okLookups[i++];
        map.emplace(pair.first, pair.second);
    }
    i = 0;
    for (auto _ : state) {
        state.PauseTiming();
        auto& pair = okLookups[i++];
        state.ResumeTiming();

        auto p = map.find(pair.first);
        benchmark::DoNotOptimize(p);
    }
    benchmark::DoNotOptimize(map);
}

void HashMapUnsuccessfullLookUp(benchmark::State& state)
{
    const auto NB = state.max_iterations;
    HashMap<int, int> map;
    std::unordered_set<int> booking;
    usize i = 0;
    for (i = 0; i < NB; i++) {
        auto v = GetRandomInt();
        booking.emplace(v);
        usize out = v + 1;
        while(booking.contains(out)){
            out = GetRandomInt();
        }
        map.Emplace(v, 2 * v);
        nonOkLookups.emplace_back(v, out);
    }
    i = 0;
    for (auto _ : state) {
        state.PauseTiming();
        auto& n = nonOkLookups[i++];
        state.ResumeTiming();

        auto p = map.Find(n.second);
        benchmark::DoNotOptimize(p);
    }
    benchmark::DoNotOptimize(map);
}

void StdHashMapUnsuccessfullLookUp(benchmark::State& state)
{
    const auto NB = state.max_iterations;
    StdHashMap<int, int> map;
    usize i = 0;
    for (i = 0; i < NB; i++) {
        auto& n = nonOkLookups[i];
        map.emplace(n.first, 2 * n.first);
    }
    i = 0;
    for (auto _ : state) {
        state.PauseTiming();
        auto& n = nonOkLookups[i++];
        state.ResumeTiming();

        auto p = map.find(n.second);
        benchmark::DoNotOptimize(p);
    }
    benchmark::DoNotOptimize(map);
}

void HashMapErease(benchmark::State& state)
{
    const auto NB = state.max_iterations;
    HashMap<int, int> map;
    usize i = 0;
    for (; i < NB; i++) {
        auto k = GetRandomInt();
        auto v = GetRandomInt();
        map.Emplace(k, v);
        erase.emplace_back(k, v);
    }
    i = 0;
    for (auto _ : state) {
        state.PauseTiming();
        auto& pair = erase[i++];
        state.ResumeTiming();

        map.Erase(pair.first);
    }
    benchmark::DoNotOptimize(map);
}

void StdHashMapErease(benchmark::State& state)
{
    const auto NB = state.max_iterations;
    StdHashMap<int, int> map;
    usize i = 0;
    for (; i < NB; i++) {
        map.emplace(erase[i].first, erase[i].second);
    }
    i = 0;
    for (auto _ : state) {
        state.PauseTiming();
        auto& pair = erase[i++];
        state.ResumeTiming();

        map.erase(pair.first);
    }
    benchmark::DoNotOptimize(map);
}*/


#endif // HASHMAP_CPP
