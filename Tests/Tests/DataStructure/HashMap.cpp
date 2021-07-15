#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include <random>
#include <Core/DataStructure/HashMap.hpp>
#include <Core/DataStructure/spe/bytell_hash_map.hpp>

constexpr auto TEST_ITERATIONS = 100'000;
using namespace TRE;

template<typename K, typename V>
using HM =
    HashMap<K,V>;
    // ska::bytell_hash_map<K, V>;

int32 GetRandomInt(int32 start = 0, int32 end = INT32_MAX - 1)
{
    std::random_device rd; // obtain a random number from hardware
    std::mt19937 gen(rd()); // seed the generator
    std::uniform_int_distribution<> dist2(start, end); // define the range
    return dist2(gen);
}

template<typename K, typename V>
auto StrictTestHashmaps(const HM<K,V>& x, const std::unordered_map<K,V>& y)
{
    ASSERT_EQ(x.Size(), y.size());
    usize index = 0;

    // Is Y in X
    for (const auto& p : y) {
        const auto& k = p.first;
        const auto& v = p.second;
        auto res = x.Find(k);
        SCOPED_TRACE(index); index++;
        ASSERT_EQ(res->first(), k);
        ASSERT_EQ(res->second(), v);
    }

    // Is X in Y
    index = 0;
    for (const auto& p : x) {
        const auto& k = p.first();
        const auto& v = p.second();
        auto res = y.find(k);
        SCOPED_TRACE(index); index++;
        ASSERT_EQ(res->first, k);
        ASSERT_EQ(res->second, v);
    }
}


template<typename K, typename V>
auto TestHashmaps(const HM<K,V>& x, const std::unordered_map<K,V>& y, usize testsCount = 10'000)
{
    StrictTestHashmaps(x, y);

    for (usize i = 0; i < testsCount; i++){
        auto k = GetRandomInt();
        auto res = y.find(k);
        auto res2 = x.Find(k);

        SCOPED_TRACE(k);
        if (res != y.end()) {
            ASSERT_EQ(res->first, res2->first());
            ASSERT_EQ(res->second, res2->second());
        }else{
            ASSERT_EQ(res2, x.end());
        }
    }
}

TEST(HashMapTest, Declaration)
{
    HM<int, int> map;
    std::unordered_map<int, int> map2;
    TestHashmaps(map, map2);
}

TEST(HashMapTest, Insertion)
{
    HM<int, int> map;
    std::unordered_map<int, int> map2;

    for (int i = 0; i < 1'000; i++){
        auto n = GetRandomInt(0, 100);
        auto i1 = map.Emplace(n, 2 * n);
        auto i2 = map2.emplace(n, 2 * n);
        ASSERT_EQ(i1.second, i2.second);
        ASSERT_EQ(i1.first->first(), n);
        ASSERT_EQ(i1.first->second(), 2 * n);
    }

    TestHashmaps(map, map2);
}

TEST(HashMapTest, RandomInsertion)
{
    constexpr auto NB = 2'000;
    HM<int, int> map;
    std::unordered_map<int, int> map2;

    for (int i = 0; i < NB; i++){
        auto k = GetRandomInt();
        auto v = GetRandomInt();
        const auto i1 = map.Emplace(k, v);
        const auto i2 = map2.emplace(k, v);
        ASSERT_EQ(i1.second, i2.second);
        TestHashmaps(map, map2, 0);
    }

    TestHashmaps(map, map2);
}


TEST(HashMapTest, Erease)
{
    constexpr auto NB = 1'000;
    HM<int, int> map;
    std::unordered_map<int, int> map2;
    std::vector<int> keys;

    for (int i = 0; i < NB; i++){
        auto k = GetRandomInt();
        map.Emplace(k, k);
        map2.emplace(k, k);
        keys.emplace_back(k);
    }

    TestHashmaps(map, map2);

    for (usize i = 0; i < keys.size() / 2; i++){
        map.Erase(keys[i]);
        map2.erase(keys[i]);
        TestHashmaps(map, map2, 0);
    }

    TestHashmaps(map, map2);

    for (usize i = keys.size() / 2; i < keys.size(); i++){
        map.Erase(keys[i]);
        map2.erase(keys[i]);
        TestHashmaps(map, map2, 0);
    }

    TestHashmaps(map, map2);
}

TEST(HashMapTest, EreaseInsertTest)
{
    constexpr auto NB = 500;
    HM<int, int> map;
    std::unordered_map<int, int> map2;
    std::vector<int> keys;

    for (int i = 0; i < NB; i++) {
        auto keysCount = keys.size();
        auto percent = GetRandomInt(0, 100);

        if (percent <= 50 || keysCount <= 0) {
            auto addCount = GetRandomInt(1, NB/10);

            for (int i = 0; i < addCount; i++){
                auto k = GetRandomInt();
                map.Emplace(k, k);
                map2.emplace(k, k);
                keys.emplace_back(k);
            }

            TestHashmaps(map, map2, 100);
        }else{
            auto removeCount = GetRandomInt(1, keysCount);

            for (int32 i = 0; i < removeCount; i++) {
                int k = keys.back();
                map.Erase(k);
                map2.erase(k);
                keys.pop_back();
                TestHashmaps(map, map2, 100);
            }
        }
    }
}


