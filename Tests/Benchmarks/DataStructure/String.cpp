#include <random>
#include <benchmark/benchmark.h>
#include <Core/DataStructure/String2.hpp>

using namespace TRE;

std::string GenerateRandomString(uint32 len)
{
    std::random_device rd; // obtain a random number from hardware
    std::mt19937 gen(rd()); // seed the generator
    std::uniform_int_distribution<> dist(32, 127); // define the range
    std::uniform_int_distribution<> dist2(0, len); // define the range

    std::string str;
    for (int i = 0; i < dist2(gen); i++) {
        str.push_back(dist(gen));
    }

    return str;
}

void StringEmptyDecl(benchmark::State& state)
{
    for (auto _ : state) {
        String2 str;
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

        String2 str(s.c_str(), s.size());
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

        String2 str(s.c_str(), s.size());
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
    String2 str;

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

// Register the function as a benchmark
BENCHMARK(StringEmptyDecl);
BENCHMARK(StdStringEmptyDecl);

BENCHMARK(StringSsoDecl);
BENCHMARK(StdStringSsoDecl);

BENCHMARK(StringDecl);
BENCHMARK(StdStringDecl);

BENCHMARK(StringAppend);
BENCHMARK(StdStringAppend);
