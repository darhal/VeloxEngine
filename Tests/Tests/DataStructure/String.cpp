#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include <random>
#include <cstring>
#include <Core/DataStructure/String2.hpp>

using namespace TRE;

std::string GenerateRandomString(uint32 len, uint32 start = 0)
{
    std::random_device rd; // obtain a random number from hardware
    std::mt19937 gen(rd()); // seed the generator
    std::uniform_int_distribution<> dist(32, 127); // define the range
    std::uniform_int_distribution<> dist2(start, len); // define the range

    std::string str;
    for (int i = 0; i < dist2(gen); i++) {
        str.push_back(dist(gen));
    }

    return str;
}

template<typename T>
auto TestString(const BasicString2<T>& x, const std::basic_string<T>& y)
{
    ASSERT_EQ(x.Size(), y.size());

    for (usize i = 0; i < x.Size(); i++) {
        ASSERT_EQ(x[i], y[i]);
    }
}

template<typename T>
auto TestCString(const BasicString2<T>& x, const T* str)
{
    usize sz = strlen(str);
    ASSERT_EQ(x.Size(), sz);

    for (usize i = 0; i < x.Size(); i++) {
        ASSERT_EQ(x[i], str[i]);
    }
}

TEST(StringsTest, Declaration)
{
    String2 str1;
    std::string str2;
    TestString(str1, str2);
}

TEST(StringsTest, Declaration2)
{
    String2 str1("Small String");
    std::string str2("Small String");
    TestString(str1, str2);
}

TEST(StringsTest, Declaration3)
{
    String2 str1("Not so small String");
    std::string str2("Not so small String");
    TestString(str1, str2);
}

TEST(StringsTest, Append)
{
    const char* APPENDS[] = {"Hello ", ",", " World ! ", "Can we add more characters please ?"};
    // const char* FINAL_PRODUCT[] = {"Hello ", "Hello ,", "Hello , World ! ", "Hello , World ! Can we add more characters please ?"};
    const usize SIZE = sizeof(APPENDS) / sizeof(APPENDS[0]);

    String2 str1;
    std::string str2;

    for (uint i = 0; i < SIZE; i++) {
        str1.Append(APPENDS[i]);
        str2.append(APPENDS[i]);

        TestString(str1, str2);
        // TestCString(str1, FINAL_PRODUCT[i]);
    }
}

TEST(StringsTest, Append2)
{
    constexpr auto NB = 1000;
    std::random_device rd; // obtain a random number from hardware
    std::mt19937 gen(rd()); // seed the generator
    std::uniform_int_distribution<> dist(32, 127); // define the range

    String2 str1;
    std::string str2;

    for (uint i = 0; i < NB; i++) {
        char ch = dist(gen);
        str1.Append(ch);
        str2.push_back(ch);

        TestString(str1, str2);
    }
}

TEST(StringsTest, Append3)
{
    constexpr auto NB = 1000;
    String2 str1;
    std::string str2;

    for (uint i = 0; i < NB; i++) {
        auto s = GenerateRandomString(1024);
        str1.Append(s.c_str(), s.size());
        str2.append(s.c_str());

        TestString(str1, str2);
    }
}

TEST(StringsTest, PopBack)
{
    constexpr auto NB = 1000;

    for (uint i = 0; i < NB; i++) {
        auto s = GenerateRandomString(1024, 1);
        String2 str1(s.c_str(), s.size());
        std::string str2(s.c_str());

        for (usize i = 0; i < s.size(); i++) {
            TestString(str1, str2);

            str1.PopBack();
            str2.pop_back();
        }

        TestString(str1, str2);
    }
}

TEST(StringsTest, InsertChar)
{
    constexpr auto NB = 1000;
    std::random_device rd; // obtain a random number from hardware
    std::mt19937 gen(rd()); // seed the generator
    std::uniform_int_distribution<> dist(32, 127); // define the range
    std::uniform_int_distribution<> dist2(0, NB); // define the range

    String2 str1;
    std::string str2;

    for (uint i = 0; i < NB; i++) {
        char ch = dist(gen);
        usize pos = str2.size() ? dist2(gen) % str2.size() : 0;

        str1.Insert(pos, ch);
        str2.insert(str2.begin() + pos, ch);

        // printf("%s - %s - %c\n", str1.Data(), str2.c_str(), ch);
        TestString(str1, str2);
    }
}

TEST(StringsTest, InsertString)
{
    constexpr auto NB = 1000;
    std::random_device rd; // obtain a random number from hardware
    std::mt19937 gen(rd()); // seed the generator
    std::uniform_int_distribution<> dist2(0, NB); // define the range

    String2 str1;
    std::string str2;

    for (uint i = 0; i < NB; i++) {
        usize pos = str2.size() ? dist2(gen) % str2.size() : 0;
        auto s = GenerateRandomString(1024, 1);

        str1.Insert(pos, s.c_str(), s.size());
        str2.insert(pos, s, 0, s.size());
        TestString(str1, str2);
    }
}

TEST(StringsTest, EraseString)
{
    constexpr auto NB = 1000;
    std::random_device rd; // obtain a random number from hardware
    std::mt19937 gen(rd()); // seed the generator
    std::uniform_int_distribution<> dist(0, NB); // define the range

    for (uint i = 0; i < NB; i++) {
        auto s = GenerateRandomString(1024, 1);
        String2 str1(s.c_str(), s.size());
        std::string str2(s.c_str());

        usize start = dist(gen) % s.size();
        usize count = dist(gen) % (s.size() - start);

        str1.Erease(start, count);
        str2.erase(start, count);
        TestString(str1, str2);
    }

    for (uint i = 0; i < NB; i++) {
        auto s = GenerateRandomString(1024, 1);
        String2 str1(s.c_str(), s.size());
        std::string str2(s.c_str());

        usize start = dist(gen) % s.size();
        usize count = dist(gen) % s.size();

        str1.Erease(start, count);
        str2.erase(start, count);
        TestString(str1, str2);
    }
}

TEST(StringsTest, StartsWith)
{
    constexpr auto NB = 1000;

    for (uint i = 0; i < NB; i++) {
        auto s = GenerateRandomString(1024);
        String2 str1(s.c_str(), s.size());

        for (usize i = 0; i < s.size(); i++) {
            ASSERT_TRUE(str1.StartsWith(str1.Data(), i));
        }
    }
}

TEST(StringsTest, EndsWith)
{
    constexpr auto NB = 1000;

    for (uint i = 0; i < NB; i++) {
        auto s = GenerateRandomString(1024);
        String2 str1(s.c_str(), s.size());

        for (usize i = s.size(); i > 0; i--) {
            ASSERT_TRUE(str1.EndsWith(str1.Data() + i, s.size() - i));
        }
    }
}

TEST(StringsTest, SubString)
{
    constexpr auto NB = 1000;
    std::random_device rd; // obtain a random number from hardware
    std::mt19937 gen(rd()); // seed the generator
    std::uniform_int_distribution<> dist(0, NB); // define the range

    for (uint i = 0; i < NB; i++) {
        auto s = GenerateRandomString(1024, 1);
        String2 str1(s.c_str(), s.size());
        std::string str2(s.c_str());

        usize start = dist(gen) % s.size();
        usize count = dist(gen) % (s.size() - start);

        auto res1 = str1.SubString(start, count);
        auto res2 = str2.substr(start, count);
        TestString(res1, res2);
    }

    for (uint i = 0; i < NB; i++) {
        auto s = GenerateRandomString(1024, 1);
        String2 str1(s.c_str(), s.size());
        std::string str2(s.c_str());

        usize start = dist(gen) % s.size();
        usize count = dist(gen) % s.size();

        auto res1 = str1.SubString(start, count);
        auto res2 = str2.substr(start, count);
        TestString(res1, res2);
    }
}

