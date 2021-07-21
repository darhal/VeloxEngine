#include <random>
#include <benchmark/benchmark.h>
#include <Core/DataStructure/Vector.hpp>
#include <Core/Memory/LocalAllocator.hpp>

using namespace TRE;

//template<typename T>
//using SmallVector = Vector<T, LocalAllocator<2 * 4096 * 4>>;

template<typename T>
using SmallVector = Vector<T>;

void VectorEmplaceBack(benchmark::State& state)
{
    SmallVector<int> vec;

    for (auto _ : state) {
        vec.EmplaceBack(5);
    }
}

void StdVectorEmplaceBack(benchmark::State& state)
{
    state.iterations();
    std::vector<int> vec;

    for (auto _ : state) {
        vec.emplace_back(5);
    }
}

void VectorEmplaceFront(benchmark::State& state)
{
    SmallVector<int> vec;
    // vec.Reserve(vec.DEFAULT_CAPACITY);

    for (auto _ : state) {
        vec.EmplaceFront(5);
    }
}

void StdVectorEmplaceFront(benchmark::State& state)
{
    state.iterations();
    std::vector<int> vec;

    for (auto _ : state) {
        vec.insert(vec.begin(), 5);
    }
}

void VectorInsert(benchmark::State& state)
{
    const auto NB = state.max_iterations;
    SmallVector<int> vec;
    int f = 0;
    int s = 1;
    vec.Insert(f, f);

    for (auto _ : state) {
        auto idx = f % vec.Size();
        vec.Insert(idx, idx);
        f = s;
        s = f + s;
    }
}

void StdVectorInsert(benchmark::State& state)
{
    const auto NB = state.max_iterations;
    std::vector<int> vec;
    int f = 0;
    int s = 1;
    vec.insert(vec.begin() + f, f);

    for (auto _ : state) {
        auto idx = f % vec.size();
        vec.insert(vec.begin() + idx, idx);
        f = s;
        s = f + s;
    }
}

void VectorErease(benchmark::State& state)
{
    const auto NB = state.max_iterations;
    SmallVector<int> vec;
    int f = 0;
    int s = 1;

    for (usize i = 0; i < NB; i++) {
        vec.EmplaceBack(f);
        f = s;
        s = f + s;
    }

    f = 0, s = 1;
    for (auto _ : state) {
        vec.Erease(f % vec.Size());
        f = s;
        s = f + s;
    }
}

void StdVectorErease(benchmark::State& state)
{
    const auto NB = state.max_iterations;
    std::vector<int> vec;
    int f = 0;
    int s = 1;

    for (usize i = 0; i < NB; i++) {
        vec.emplace_back(f);
        f = s;
        s = f + s;
    }

    f = 0, s = 1;
    for (auto _ : state) {
        vec.erase(vec.begin() + (f % vec.size()));
        f = s;
        s = f + s;
    }
}

void VectorEreaseRange(benchmark::State& state)
{
    const auto NB = state.max_iterations;
    SmallVector<int> vec;
    int f = 0;
    int s = 1;

    for (usize i = 0; i < 10'000 * NB; i++) {
        vec.EmplaceBack(f);
        f = s;
        s = f + s;
    }

    f = 0, s = 1;
    for (auto _ : state) {
        state.PauseTiming();
        auto start = f % NB;
        auto end = s % NB;
        if (start > end)
            std::swap(start, end);
        f = s;
        s = f + s;
        state.ResumeTiming();

        vec.Erease(start, end);
    }
}

void StdVectorEreaseRange(benchmark::State& state)
{
    const auto NB = state.max_iterations;
    std::vector<int> vec;
    int f = 0;
    int s = 1;

    for (usize i = 0; i < 10'000 * NB; i++) {
        vec.emplace_back(s);
        f = s;
        s = f + s;
    }

    f = 0, s = 1;
    for (auto _ : state) {
        state.PauseTiming();
        auto start = f % NB;
        auto end = s % NB;
        if (start > end)
            std::swap(start, end);
        f = s;
        s = f + s;
        state.ResumeTiming();

        vec.erase(vec.begin() + start, vec.begin() + end);
    }
}


// Register the function as a benchmark
BENCHMARK(VectorEmplaceBack)        ;//->Iterations(1'000'000);
BENCHMARK(StdVectorEmplaceBack)     ;//->Iterations(1'000'000);
                                    ;//
BENCHMARK(VectorEmplaceFront)       ;//->Iterations(250'000);
BENCHMARK(StdVectorEmplaceFront)    ;//->Iterations(250'000);
                                    ;//
BENCHMARK(VectorInsert)             ;//->Iterations(200'000);
BENCHMARK(StdVectorInsert)          ;//->Iterations(200'000);
                                    ;//
BENCHMARK(VectorErease)             ;//->Iterations(50'000);
BENCHMARK(StdVectorErease)          ;//->Iterations(50'000);
                                    ;//
BENCHMARK(VectorEreaseRange)        ;//->Iterations(25'000);
BENCHMARK(StdVectorEreaseRange)     ;//->Iterations(25'000);

/*std::vector<usize> insertPos;
std::vector<usize> erasePos;
std::vector<std::pair<usize, usize>> ereaseRange;

void VectorInsert(benchmark::State& state)
{
    const auto NB = state.max_iterations;
    std::random_device rd; // obtain a random number from hardware
    std::mt19937 gen(rd()); // seed the generator
    std::uniform_int_distribution<> distr(0, NB); // define the range

    SmallVector<int> vec;
    // vec.Reserve(vec.DEFAULT_CAPACITY);
    int pos = 0;

    for (usize i = 0; i < NB; i++) {
        insertPos.emplace_back(pos);
        pos = distr(gen) % (i + 1);
    }

    pos = 0;

    for (auto _ : state) {
        state.PauseTiming();
        usize index = insertPos[pos++];
        state.ResumeTiming();

        vec.Insert(index, index);
    }
}

void StdVectorInsert(benchmark::State& state)
{
    std::vector<int> vec;
    usize pos = 0;

    for (auto _ : state) {
        state.PauseTiming();
        usize index = insertPos[pos++];
        state.ResumeTiming();

        vec.insert(vec.begin() + index, index);
    }
}

void VectorErease(benchmark::State& state)
{
    const auto NB = state.max_iterations;
    std::random_device rd; // obtain a random number from hardware
    std::mt19937 gen(rd()); // seed the generator
    std::uniform_int_distribution<> distr(0, NB); // define the range
    usize pos1 = 0;

    SmallVector<int> vec;

    for (usize i = 0; i < NB; i++) {
        erasePos.emplace_back(pos1);
        vec.EmplaceBack(i);
        pos1 = distr(gen) % (i + 1);
    }

    pos1 = 0;

    for (auto _ : state) {
        state.PauseTiming();
        usize index = erasePos[pos1++] % vec.Size();
        state.ResumeTiming();

        vec.Erease(index);
    }
}

void StdVectorErease(benchmark::State& state)
{
    const auto NB = state.max_iterations;
    std::vector<int> vec;
    usize pos1 = 0;

    for (usize i = 0; i < NB; i++) {
        vec.emplace_back(i);
    }

    for (auto _ : state) {
        state.PauseTiming();
        usize index = erasePos[pos1++] % vec.size();
        state.ResumeTiming();

        vec.erase(vec.begin() + index);
    }
}

void VectorEreaseRange(benchmark::State& state)
{
    const auto NB = state.max_iterations;
    std::random_device rd; // obtain a random number from hardware
    std::mt19937 gen(rd()); // seed the generator
    std::uniform_int_distribution<> distr(0, NB); // define the range
    usize pos1 = 0;
    usize pos2 = 0;

    for (usize i = 0; i < NB; i++) {
        pos1 = distr(gen) % (i+1);
        pos2 = distr(gen) % (i+1);

        if (pos1 > pos2)
            std::swap(pos1, pos2);

        ereaseRange.emplace_back(pos1, pos2);
    }

    pos1 = 0;
    SmallVector<int> vec;

    for (auto _ : state) {
        state.PauseTiming();
        vec.Clear();
        for (usize i = 0; i < NB; i++)
            vec.EmplaceBack(i);
        auto p = ereaseRange[pos1++];
        state.ResumeTiming();

        vec.Erease(p.first, p.second);
    }
}

void StdVectorEreaseRange(benchmark::State& state)
{
    const auto NB = state.max_iterations;
    usize index = 0;
    std::vector<int> vec;
    for (auto _ : state) {
        state.PauseTiming();
        vec.clear();
        for (usize i = 0; i < NB; i++)
            vec.emplace_back(i);
        auto p = ereaseRange[index++];
        state.ResumeTiming();

        vec.erase(vec.begin() + p.first, vec.begin() + p.second);
    }
}

*/
