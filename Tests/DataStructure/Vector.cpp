#include <gtest/gtest.h>
#include <gmock/gmock.h>
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

TEST(VectorTests, TestEmplaceBack)
{
    using namespace TRE;
    Vector<int> x;
    std::vector<int> y;

    for (int i = 0; i < 100; i++) {
        x.EmplaceBack(i);
        y.emplace_back(i);
    }

    TestVectors(x, y);
}

TEST(VectorTests, TestPusBack)
{
    using namespace TRE;
    Vector<int> x;
    std::vector<int> y;

    for (int i = 0; i < 100; i++) {
        x.PushBack(i);
        y.push_back(i);
    }

    TestVectors(x, y);
}

TEST(VectorTests, TestInsert)
{
    using namespace TRE;
    Vector<int> x;
    std::vector<int> y;

    for (int i = 0; i < 100; i++) {
        x.Insert(i, i);
        y.insert(y.begin() + i, i);
    }

    TestVectors(x, y);
}

