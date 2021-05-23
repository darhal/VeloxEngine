#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include <random>
#include <Core/DataStructure/Vector/Vector.hpp>

template<typename T1, typename T2>
auto TestVectors(const T1& x, const T2& y)
{
    ASSERT_EQ(x.Size(), y.size());

    for (usize idx = 0; idx < x.Size(); idx++) 
    {
        SCOPED_TRACE(idx);
        ASSERT_EQ(x[idx], y[idx]);
    }
}

TEST(VectorTests, EmplaceBack)
{
    constexpr auto NB = 100;
    using namespace TRE;
    Vector<int> x;
    std::vector<int> y;

    for (int i = 0; i < NB; i++) {
        x.EmplaceBack(i);
        y.emplace_back(i);
        TestVectors(x, y);
    }
}

TEST(VectorTests, PushBack)
{
    constexpr auto NB = 100;
    using namespace TRE;
    Vector<int> x;
    std::vector<int> y;

    for (int i = 0; i < NB; i++) {
        x.PushBack(i);
        y.push_back(i);
        TestVectors(x, y);
    }
}

TEST(VectorTests, EmplaceFront)
{
    constexpr auto NB = 100;
    using namespace TRE;
    Vector<int> x;
    std::vector<int> y;

    for (int i = 0; i < NB; i++) {
        x.EmplaceFront(i);
        y.emplace(y.begin(), i);
        TestVectors(x, y);
    }
}

TEST(VectorTests, PushFront)
{
    constexpr auto NB = 100;
    using namespace TRE;
    Vector<int> x;
    std::vector<int> y;

    for (int i = 0; i < NB; i++) {
        x.PushFront(i);
        y.insert(y.begin(), i);
        TestVectors(x, y);
    }
}

TEST(VectorTests, Insert)
{
    constexpr auto NB = 100;
    std::random_device rd; // obtain a random number from hardware
    std::mt19937 gen(rd()); // seed the generator
    std::uniform_int_distribution<> distr(0, NB); // define the range

    using namespace TRE;
    Vector<int> x;
    std::vector<int> y;
    int pos = 0;

    for (int i = 0; i < NB; i++) {
        x.Insert(pos, i);
        y.insert(y.begin() + pos, i);
        pos = distr(gen) % x.Size();
        TestVectors(x, y);
    }
}

TEST(VectorTests, Emplace)
{
    constexpr auto NB = 100;
    std::random_device rd; // obtain a random number from hardware
    std::mt19937 gen(rd()); // seed the generator
    std::uniform_int_distribution<> distr(0, NB); // define the range

    using namespace TRE;
    Vector<int> x;
    std::vector<int> y;
    int pos = 0;

    for (int i = 0; i < NB; i++) {
        x.Emplace(pos, i);
        y.emplace(y.begin() + pos, i);
        pos = distr(gen) % x.Size();
        TestVectors(x, y);
    }
}

TEST(VectorTests, PopBack)
{
    constexpr auto NB = 100;
    std::random_device rd; // obtain a random number from hardware
    std::mt19937 gen(rd()); // seed the generator
    std::uniform_int_distribution<> distr(0, NB); // define the range

    using namespace TRE;
    Vector<int> x;
    std::vector<int> y;
    int pos = 0;

    for (int i = 0; i < NB; i++) {
        x.Emplace(pos, i);
        y.emplace(y.begin() + pos, i);
        pos = distr(gen) % x.Size();
    }

    TestVectors(x, y);

    for (int i = 0; i < NB; i++) {
        x.PopBack();
        y.pop_back();
        TestVectors(x, y);
    }
}

TEST(VectorTests, PopFront)
{
    constexpr auto NB = 100;
    std::random_device rd; // obtain a random number from hardware
    std::mt19937 gen(rd()); // seed the generator
    std::uniform_int_distribution<> distr(0, NB); // define the range

    using namespace TRE;
    Vector<int> x;
    std::vector<int> y;
    int pos = 0;

    for (int i = 0; i < NB; i++) {
        x.Emplace(pos, i);
        y.emplace(y.begin() + pos, i);
        pos = distr(gen) % x.Size();
    }

    TestVectors(x, y);

    for (int i = 0; i < NB; i++) {
        x.PopFront();
        y.erase(y.begin());
        TestVectors(x, y);
    }
}

TEST(VectorTests, EreaseOne)
{
    constexpr auto NB = 1000;
    std::random_device rd; // obtain a random number from hardware
    std::mt19937 gen(rd()); // seed the generator
    std::uniform_int_distribution<> distr(0, NB); // define the range

    using namespace TRE;
    Vector<int> x;
    std::vector<int> y;
    int pos = 0;

    for (int i = 0; i < NB; i++) {
        x.Insert(pos, i);
        y.insert(y.begin() + pos, i);
        pos = distr(gen) % x.Size();
    }

    TestVectors(x, y);

    for (int i = 0; i < NB; i++) {
        pos = distr(gen) % y.size();
        x.Erease(pos);
        y.erase(y.begin() + pos);
        TestVectors(x, y);
    }
}

TEST(VectorTests, EreaseRange)
{
    constexpr auto NB = 750;
    std::random_device rd; // obtain a random number from hardware
    std::mt19937 gen(rd()); // seed the generator
    std::uniform_int_distribution<> distr(0, NB); // define the range

    using namespace TRE;
    Vector<int> x;
    std::vector<int> y;
    usize pos1 = 0;
    usize pos2 = 0;

    for (int i = 0; i < NB; i++) {
        x.Insert(pos1, i);
        y.insert(y.begin() + pos1, i);
        pos1 = distr(gen) % x.Size();
    }

    TestVectors(x, y);

    for (int i = 0; i < NB; i++) {
        pos1 = distr(gen) % y.size();
        pos2 = distr(gen) % y.size();

        if (pos1 > pos2) {
            std::swap(pos1, pos2);
        }

        x.Erease(pos1, pos2);
        y.erase(y.begin() + pos1, y.begin() + pos2);
        TestVectors(x, y);
    }
}


// TODO: Test EreaseFast, InsertFast, Copy ctor, move ctor, etc...