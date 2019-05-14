#include <Core/DataStructure/String/String.hpp>
#include <Core/DataStructure/Array/Array.hpp>
#include <Core/Memory/Common.hpp>
#include <iostream>
#include <cstdio>
#include <stdlib.h>
#include <chrono>
#include <bitset>
#include <array>

using namespace TRE;

void TestStackAlloc();
void TestAlignedStackAlloc();
void TestLinearAlloc();
void TestPoolAlloc();
void BenchmarkStdString();
void BenchmarkString();

int main()
{
	/*TRE::BasicString<char> str("Hello friend.");
	printf("%s | Length = %d\n", str.Buffer(), str.Length());
	str.Insert(5, " there");
	printf("%s | Length = %d\n", str.Buffer(), str.Length());*/
	/*BenchmarkStdString();
	printf("\n\n\n");
	BenchmarkString();*/
	auto start = std::chrono::steady_clock::now();
	std::array<std::string, 5> arr = { "Hi", "Hello How are you .?", "I'm Fine thank you." } ;
	auto end = std::chrono::steady_clock::now();
	auto diff = end - start;
	std::cout << "std::array benchmark of declartion of std::string { Hi, Hello How are you .?, I'm Fine thank you. } :" << std::chrono::duration<double, std::nano>(diff).count() << " ns" << std::endl;

	start = std::chrono::steady_clock::now();
	Array<String, 5> strArray = { "Hi", "Hello How are you .?", "I'm Fine thank you." };
	end = std::chrono::steady_clock::now();
	diff = end - start;
	std::cout << "TRE::Array benchmark of declartion of TRE::String { Hi, Hello How are you .?, I'm Fine thank you. } :" << std::chrono::duration<double, std::nano>(diff).count() << " ns" << std::endl;

	strArray.EmplaceBack("Fuck you");

	arr.assign()
	strArray.PopBack();
	for (const String& s : strArray) {
		printf("%s\n", s.Buffer());
	}
	printf("Len = %d\n", strArray.Length());
	strArray.Clear();

	Array<int, 5> intArr = { 5, 6, 8, 7 };
	for (const int& s : intArr) {
		printf("%d\n", s);
	}
	printf("Len = %d\n", intArr.Length());
	strArray.Clear();
	intArr.Clear();
	
	printf("\n\n");
	getchar();
	return 0;
}

void BenchmarkString()
{
	printf("----------------- TRE STRING -----------------\n");
	auto start = std::chrono::steady_clock::now();
	String text("ABCDEF");
	auto end = std::chrono::steady_clock::now();
	auto diff = end - start;
	printf("text = %s\n", text.Buffer());
	std::cout << "TRE::String benchmark of declartion of text(ABCDEF) :" << std::chrono::duration<double, std::nano>(diff).count() << " ns" << std::endl;

	printf("----------------- TRE STRING -----------------\n");
	start = std::chrono::steady_clock::now();
	String text2("BCD");
	end = std::chrono::steady_clock::now();
	diff = end - start;
	printf("text3 = %s\n", text2.Buffer());
	std::cout << "TRE::String benchmark of declartion of text2(BCD) :" << std::chrono::duration<double, std::nano>(diff).count() << " ns" << std::endl;

	printf("text = %s | text2 = %s\n", text.Buffer(), text2.Buffer());
	start = std::chrono::steady_clock::now();
	ssize slot = text.Find(text2);
	end = std::chrono::steady_clock::now();
	diff = end - start;
	std::cout << "TRE::String benchmark of text.find(text2) :" << std::chrono::duration<double, std::nano>(diff).count() << " ns" << std::endl;
	printf("Pattren found at : %d\n", slot);

	printf("----------------- TRE STRING -----------------\n");
	start = std::chrono::steady_clock::now();
	String text3("ABAAABCD");
	end = std::chrono::steady_clock::now();
	diff = end - start;
	printf("text3 = %s\n", text3.Buffer());
	std::cout << "TRE::String benchmark of declartion of text3(ABAAABCD) :" << std::chrono::duration<double, std::nano>(diff).count() << " ns" << std::endl;

	printf("text = %s | text3 = %s\n", text.Buffer(), text3.Buffer());
	start = std::chrono::steady_clock::now();
	slot = text == text3;
	end = std::chrono::steady_clock::now();
	diff = end - start;
	std::cout << "TRE::String benchmark of text == text3 :" << std::chrono::duration<double, std::nano>(diff).count() << " ns" << std::endl;
	printf("Result : %s\n", (slot) ? "true" : "false");

	
	printf("Appending ' XBC'\n");
	start = std::chrono::steady_clock::now();
	text.Append(String(" XBC"));
	end = std::chrono::steady_clock::now();
	diff = end - start;
	std::cout << "TRE::String benchmark of Append(' XBC'); :" << std::chrono::duration<double, std::nano>(diff).count() << " ns" << std::endl;
	printf("Text = %s\n", text.Buffer());

	printf("text = %s (Erease(%d, %d))\n", text.Buffer(), 4, 6);
	start = std::chrono::steady_clock::now();
	text.Erase(4, 6);
	end = std::chrono::steady_clock::now();
	diff = end - start;
	std::cout << "TRE::String benchmark of Erease(4, 6); :" << std::chrono::duration<double, std::nano>(diff).count() << " ns" << std::endl;
	printf("Text = %s\n", text.Buffer());

}

void BenchmarkStdString()
{
	printf("----------------- STD STRING -----------------\n");
	auto start = std::chrono::steady_clock::now();
	std::string text("ABCDEF");
	auto end = std::chrono::steady_clock::now();
	auto diff = end - start;
	printf("text = %s\n", text.c_str());
	std::cout << "STD::String benchmark of declartion of text(ABCDEF) :" << std::chrono::duration<double, std::nano>(diff).count() << " ns" << std::endl;

	printf("----------------- STD STRING -----------------\n");
	start = std::chrono::steady_clock::now();
	std::string text2("BCD");
	end = std::chrono::steady_clock::now();
	diff = end - start;
	printf("text3 = %s\n", text2.c_str());
	std::cout << "STD::String benchmark of declartion of text2(BCD) :" << std::chrono::duration<double, std::nano>(diff).count() << " ns" << std::endl;

	printf("text = %s | text2 = %s\n", text.c_str(), text2.c_str());
	start = std::chrono::steady_clock::now();
	ssize slot = text.find(text2);
	end = std::chrono::steady_clock::now();
	diff = end - start;
	std::cout << "STD::String benchmark of text.find(text2) :" << std::chrono::duration<double, std::nano>(diff).count() << " ns" << std::endl;
	printf("Pattren found at : %llu\n", slot);

	printf("----------------- STD STRING -----------------\n");
	start = std::chrono::steady_clock::now();
	std::string text3("ABAAABCD");
	end = std::chrono::steady_clock::now();
	diff = end - start;
	printf("text3 = %s\n", text3.c_str());
	std::cout << "STD::String benchmark of declartion of text3(ABAAABCD) :" << std::chrono::duration<double, std::nano>(diff).count() << " ns" << std::endl;

	printf("text = %s | text3 = %s\n", text.c_str(), text3.c_str());
	start = std::chrono::steady_clock::now();
	slot = text == text3;
	end = std::chrono::steady_clock::now();
	diff = end - start;
	std::cout << "STD::String benchmark of text == text3 :" << std::chrono::duration<double, std::nano>(diff).count() << " ns" << std::endl;
	printf("Result : %s\n", (slot) ? "true" : "false");

	printf("Appending ' XBC'\n");
	start = std::chrono::steady_clock::now();
	text.append(" XBC");
	end = std::chrono::steady_clock::now();
	diff = end - start;
	std::cout << "STD::String benchmark of Append(' XBC'); :" << std::chrono::duration<double, std::nano>(diff).count() << " ns" << std::endl;
	printf("Text = %s\n", text.c_str());

	printf("text = %s (Erease(%d, %d))\n", text.c_str(), 4, 6);
	start = std::chrono::steady_clock::now();
	text.erase(4, 6);
	end = std::chrono::steady_clock::now();
	diff = end - start;
	std::cout << "STD::String benchmark of Erease(4, 6); :" << std::chrono::duration<double, std::nano>(diff).count() << " ns" << std::endl;
	printf("Text = %s\n", text.c_str());
}

void TestStackAlloc()
{

}

void TestAlignedStackAlloc()
{

}

void TestLinearAlloc()
{

}

void TestPoolAlloc()
{

}