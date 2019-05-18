#include <Core/DataStructure/String/String.hpp>
#include <Core/DataStructure/Array/Array.hpp>
#include <Core/Memory/Common.hpp>
#include <iostream>
#include <cstdio>
#include <stdlib.h>
#include <chrono>
#include <bitset>
#include <array>
#include <Core/Memory/RefCounter/RefCounter.hpp>
#include <Core/Memory/SmartPtr/SharedPointer.hpp>
#include <Core/DataStructure/Vector/Vector.hpp>
#include <vector>

using namespace TRE;

void TestStackAlloc();
void TestAlignedStackAlloc();
void TestLinearAlloc();
void TestPoolAlloc();
void BenchmarkStdString();
void BenchmarkString();

struct A
{
	A() { printf("ctor\n"); }
	~A() { printf("dtor\n"); }
};


template<typename T>
void PrintVec(Vector<T>& vec)
{
	printf("Len = %d | Cap = %d\n", vec.Length(), vec.Capacity());
	printf("Data = {\n");
	for (usize i = 0; i < vec.Length(); i++)
	{
		printf("[%d]=%s, ", i, (*vec.At(i)).Buffer());
	}
	printf("}\n");
	printf("--------------------------\n");
}

template<typename T>
void PrintVec(std::vector<T>& vec)
{
	printf("Len = %d | Cap = %d\n", vec.size(), vec.capacity());
	printf("Data = {\n");
	for (usize i = 0; i < vec.size(); i++)
	{
		printf("[%d]=%s, ", i, (vec.at(i)).Buffer());
	}
	printf("}\n");
	printf("--------------------------\n");
}

int main()
{
	Vector<String> arr = {"Hello", "Hi", "I can hear you", "Nice", "Sure"};
	arr.Emplace(6, "BLABLA");
	PrintVec(arr);
	arr.Erease(1, 2);
	PrintVec(arr);
	/*const usize MAX = 1;
	{
		auto start = std::chrono::steady_clock::now();
		Vector<String> arr;
		auto end = std::chrono::steady_clock::now();
		auto diff = end - start;
		std::cout << "TRE::Vector benchmark of declartion of Vector<String> arr :" << std::chrono::duration<double, std::nano>(diff).count() << " ns" << std::endl;

		double sum = 0;
		for (usize i = 0; i < MAX; i++) {
			start = std::chrono::steady_clock::now();
			arr.EmplaceBack("50");
			arr.EmplaceBack("70");
			arr.EmplaceBack("80");
			arr.EmplaceBack("90");
			arr.EmplaceBack("90");
			arr.EmplaceBack("20");
			arr.EmplaceBack("156");
			arr.EmplaceBack("6161");
			arr.EmplaceBack("965");
			arr.Emplace(3, "69");
			end = std::chrono::steady_clock::now();
			diff = end - start;
			sum += std::chrono::duration<double, std::nano>(diff).count();
		}
		std::cout << "TRE::Vector AVERAGE benchmark of pushing back 9 string elements (x" <<MAX<<"times) :" << sum / MAX  << " ns" << std::endl;
		printf("TRE::VEC Len = %d | Cap = %d\n", arr.Length(), arr.Capacity());
		PrintVec(arr);
	}
	{
		auto start = std::chrono::steady_clock::now();
		std::vector<String> arr(MAX);
		auto end = std::chrono::steady_clock::now();
		auto diff = end - start;
		std::cout << "std::vector benchmark of declartion of std::vector<String> arr :" << std::chrono::duration<double, std::nano>(diff).count() << " ns" << std::endl;

		double sum = 0;
		for (usize i = 0; i < MAX; i++) {
			start = std::chrono::steady_clock::now();
			arr.emplace_back("50");
			arr.emplace_back("70");
			arr.emplace_back("80");
			arr.emplace_back("90");
			arr.emplace_back("90");
			arr.emplace_back("20");
			arr.emplace_back("156");
			arr.emplace_back("6161");
			arr.emplace_back("965");
			arr.emplace(arr.begin()+3, "69");
			end = std::chrono::steady_clock::now();
			diff = end - start;
			sum += std::chrono::duration<double, std::nano>(diff).count();
		}
		std::cout << "std::vector AVERAGE benchmark of pushing back 9 string elements (x" << MAX << "times) :" << sum / MAX << " ns" << std::endl;
		printf("std::vec Len = %d | Cap = %d\n", arr.size(), arr.capacity());
		PrintVec(arr);
	}*/
	getchar();
	return 0;
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