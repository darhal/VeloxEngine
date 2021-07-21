#include <random>
#include <benchmark/benchmark.h>
#include <Core/DataStructure/String.hpp>

using namespace TRE;

std::string GenerateRandomString(usize len, usize start = 0)
{
    std::random_device rd; // obtain a random number from hardware
    std::mt19937 gen(rd()); // seed the generator
    std::uniform_int_distribution<> dist(32, 127); // define the range
    std::uniform_int_distribution<> dist2(start, len); // define the range
    usize end = start;
    std::string str;
    str.reserve(len);

    if (len != start) end = dist2(gen);

    for (usize i = 0; i < end; i++) {
        str.push_back(dist(gen));
    }

    return str;
}


void StringEmptyDecl(benchmark::State& state)
{
    for (auto _ : state) {
        String str;
        benchmark::DoNotOptimize(str);
    }
}

void StdStringEmptyDecl(benchmark::State& state)
{
    for (auto _ : state) {
        std::string str;
        benchmark::DoNotOptimize(str);
    }
}

void StringSsoDecl(benchmark::State& state)
{
    for (auto _ : state) {
        state.PauseTiming();
        auto s = GenerateRandomString(15);
        state.ResumeTiming();

        String str(s);
        benchmark::DoNotOptimize(str);
    }
}

void StdStringSsoDecl(benchmark::State& state)
{
    for (auto _ : state) {
        state.PauseTiming();
        auto s = GenerateRandomString(15);
        state.ResumeTiming();

        std::string str(s.c_str());
        benchmark::DoNotOptimize(str);
    }
}

void StringDecl(benchmark::State& state)
{
    for (auto _ : state) {
        state.PauseTiming();
        auto s = GenerateRandomString(1024);
        state.ResumeTiming();

        String str(s);
        benchmark::DoNotOptimize(str);
    }
}

void StdStringDecl(benchmark::State& state)
{
    for (auto _ : state) {
        state.PauseTiming();
        auto s = GenerateRandomString(1024);
        state.ResumeTiming();

        std::string str(s.c_str());
        benchmark::DoNotOptimize(str);
    }
}

void StringAppend(benchmark::State& state)
{
    String str;

    for (auto _ : state) {
        state.PauseTiming();
        auto s = GenerateRandomString(1024);
        state.ResumeTiming();

        str.Append(s.c_str(), s.size());
        benchmark::DoNotOptimize(str);
    }
}

void StdStringAppend(benchmark::State& state)
{
    std::string str;

    for (auto _ : state) {
        state.PauseTiming();
        auto s = GenerateRandomString(1024);
        state.ResumeTiming();

        str.append(s.c_str());
        benchmark::DoNotOptimize(str);
    }
}

void StringAppend2(benchmark::State& state)
{
    std::random_device rd; // obtain a random number from hardware
    std::mt19937 gen(rd()); // seed the generator
    std::uniform_int_distribution<> dist(32, 127); // define the range
    String str;

    for (auto _ : state) {
        state.PauseTiming();
        char ch = dist(gen);
        state.ResumeTiming();

        str.Append(ch);
    }

    benchmark::DoNotOptimize(str);
}

void StdStringAppend2(benchmark::State& state)
{
    std::random_device rd; // obtain a random number from hardware
    std::mt19937 gen(rd()); // seed the generator
    std::uniform_int_distribution<> dist(32, 127); // define the range
    std::string str;

    for (auto _ : state) {
        state.PauseTiming();
        char ch = dist(gen);
        state.ResumeTiming();

        str.push_back(ch);
        benchmark::DoNotOptimize(str);
    }
}

void StringPopBack(benchmark::State& state)
{
    const auto NB = state.max_iterations;
    auto s = GenerateRandomString(NB, NB);
    String str(s);

    for (auto _ : state) {
        str.PopBack();
    }
}

void StdStringPopBack(benchmark::State& state)
{
    const auto NB = state.max_iterations;
    auto s = GenerateRandomString(NB, NB);
    std::string str(s);

    for (auto _ : state) {
        str.pop_back();
    }
}

std::vector<uint32> insertPosition;
std::vector<std::pair<uint32, std::string>> insertStrPos;
std::vector<std::pair<usize, usize>> ereasePos;

void StringInsertChar(benchmark::State& state)
{
    const auto NB = state.max_iterations;
    std::random_device rd; // obtain a random number from hardware
    std::mt19937 gen(rd()); // seed the generator
    std::uniform_int_distribution<> dist2(0, NB); // define the range
    String str;
    usize idx = 0;

    for (uint i = 0; i < NB; i++) {
        insertPosition.emplace_back(idx);
        idx = dist2(gen) % (i + 1);
    }

    idx = 0;

    for (auto _ : state) {
        state.PauseTiming();
        uint pos = insertPosition[idx++];
        char ch = pos % 128 + 32;
        state.ResumeTiming();

        str.Insert(pos, ch);
    }
}

void StdStringInsertChar(benchmark::State& state)
{
    std::string str;
    usize idx = 0;

    for (auto _ : state) {
        state.PauseTiming();
        uint pos = insertPosition[idx++];
        char ch = pos % 128 + 32;
        state.ResumeTiming();

        str.insert(str.begin() + pos, ch);
    }
}

void StringInsertString(benchmark::State& state)
{
    const auto NB = state.max_iterations;
    std::random_device rd; // obtain a random number from hardware
    std::mt19937 gen(rd()); // seed the generator
    std::uniform_int_distribution<> dist2(0, NB); // define the range
    String str;
    usize idx = 0;
    usize total = 0;

    for (uint i = 0; i < NB; i++) {
        auto s = GenerateRandomString(1024, 1);
        insertStrPos.emplace_back(idx, s);
        total += s.size();
        idx = dist2(gen) % total;
    }

    idx = 0;

    for (auto _ : state) {
        state.PauseTiming();
        auto p = insertStrPos[idx++];
        state.ResumeTiming();

        str.Insert(p.first, p.second.c_str(), p.second.size());
    }
}

void StdStringInsertString(benchmark::State& state)
{
    std::string str;
    usize idx = 0;

    for (auto _ : state) {
        state.PauseTiming();
        auto p = insertStrPos[idx++];
        state.ResumeTiming();

        str.insert(p.first, p.second.c_str(), 0, p.second.size());
    }
}

void StringErease(benchmark::State& state)
{
    const auto NB = state.max_iterations;
    std::random_device rd; // obtain a random number from hardware
    std::mt19937 gen(rd()); // seed the generator
    std::uniform_int_distribution<> dist2(0, NB); // define the range
    usize idx = 0;

    for (usize i = 0; i < NB; i++) {
        usize p1 = dist2(gen) % (NB - 1);
        usize p2 = dist2(gen) % (NB - p1);
        ereasePos.emplace_back(p1, p2);
    }

    for (auto _ : state) {
        state.PauseTiming();
        auto p = ereasePos[idx++];
        auto s = GenerateRandomString(NB, NB);
        String str(s);
        state.ResumeTiming();

        str.EraseRange(p.first, p.second);
    }
}

void StdStringErease(benchmark::State& state)
{
    const auto NB = state.max_iterations;
    usize idx = 0;

    for (auto _ : state) {
        state.PauseTiming();
        auto p = ereasePos[idx++];
        auto s = GenerateRandomString(NB, NB);
        state.ResumeTiming();

        s.erase(p.first, p.second);
    }
}

// Register the function as a benchmark
//BENCHMARK(StringEmptyDecl);
//BENCHMARK(StdStringEmptyDecl);

//BENCHMARK(StringSsoDecl);
//BENCHMARK(StdStringSsoDecl);

//BENCHMARK(StringDecl);
//BENCHMARK(StdStringDecl);

//BENCHMARK(StringAppend);
//BENCHMARK(StdStringAppend);

//BENCHMARK(StringAppend2);
//BENCHMARK(StdStringAppend2);

//BENCHMARK(StringPopBack)->Iterations(1'000'000);
//BENCHMARK(StdStringPopBack)->Iterations(1'000'000);

//BENCHMARK(StringInsertChar)->Iterations(50'000);
//BENCHMARK(StdStringInsertChar)->Iterations(50'000);

//BENCHMARK(StringInsertString)->Iterations(10'000);
//BENCHMARK(StdStringInsertString)->Iterations(10'000);

//BENCHMARK(StringErease)->Iterations(10'000);
//BENCHMARK(StdStringErease)->Iterations(10'000);
