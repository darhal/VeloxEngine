#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include <random>
#include <Core/DataStructure/String2.hpp>

using namespace TRE;

template<typename T>
auto TestString(const BasicString2<T>& x, const std::basic_string<T>& y)
{
    ASSERT_EQ(x.Size(), y.size());

    for (usize i = 0; i < x.Size(); i++) {
        ASSERT_EQ(x[i], y[i]);
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
    const usize SIZE = sizeof(APPENDS) / sizeof(APPENDS[0]);

    String2 str1;
    std::string str2;

    for (uint i = 0; i < SIZE; i++) {
        str1.Append(APPENDS[i]);
        str2.append(APPENDS[i]);

        TestString(str1, str2);
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
        str1.Append(dist(gen));
        str2.push_back(dist(gen));

        TestString(str1, str2);
    }
}

TEST(StringsTest, Append3)
{

}
