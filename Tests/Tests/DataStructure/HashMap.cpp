#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include <random>
#include <Core/DataStructure/HashMap.hpp>

constexpr auto TEST_ITERATIONS = 10;
using namespace TRE;

uint32 GetRandomInt(uint32 start = 0, uint32 end = INT32_MAX - 1)
{
    std::random_device rd; // obtain a random number from hardware
    std::mt19937 gen(rd()); // seed the generator
    std::uniform_int_distribution<> dist2(start, end); // define the range
    return dist2(gen);
}

template<typename K, typename V>
auto TestHashmaps(const HashMap<K,V>& x, const std::unordered_map<K,V>& y)
{
    ASSERT_EQ(x.Size(), y.size());

    for (const auto& p : y) {
        const auto& k = p.first;
        const auto& v = p.second;
        auto res = x.Find(k);
        ASSERT_EQ((*res).first, k);
        ASSERT_EQ((*res).second, v);
    }

    for (int i = 0; i < 1'000; i++){
        auto k = GetRandomInt();
        auto res = y.find(k);
        auto res2 = x.Find(k);

        if (res != y.end()) {
            ASSERT_EQ(res->first, (*res2).first);
            ASSERT_EQ(res->second, (*res2).second);
        }else{
            ASSERT_EQ(res2, x.end());
        }
    }

    // Testing HashMap for loop
    for (const auto& p : x) {
        const auto& k = p.first;
        const auto& v = p.second;
        auto res = y.find(k);
        ASSERT_EQ(res->first, k);
        ASSERT_EQ(res->second, v);
    }
}

TEST(HashMapTest, Declaration)
{
    constexpr auto NB = TEST_ITERATIONS;
    HashMap<int, int> map;
    std::unordered_map<int, int> map2;
}


TEST(HashMapTest, Insertion)
{
    constexpr auto NB = TEST_ITERATIONS;
    HashMap<int, int> map;
    std::unordered_map<int, int> map2;

    const auto itr = map.Emplace(1, 3);
    const auto p = itr.first;
    ASSERT_EQ((*p).first, 1);
    ASSERT_EQ((*p).second, 3);
}

TEST(HashMapTest, RandomInsertion)
{
    constexpr auto NB = TEST_ITERATIONS;
    HashMap<int, int> map;
    std::unordered_map<int, int> map2;

    for (int i = 0; i < NB; i++){
        auto k = GetRandomInt();
        auto v = GetRandomInt();
        printf("Inserting [%d] = %d\n", k, v);
        map.Emplace(k, v);
        map2.emplace(k, v);
    }

    TestHashmaps(map, map2);
}



