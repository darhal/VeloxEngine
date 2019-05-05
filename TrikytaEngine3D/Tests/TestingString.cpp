#include <Core/DataStructure/String/String.hpp>
#include <Core/Memory/Common.hpp>
#include <iostream>
#include <cstdio>
#include <stdlib.h>

using namespace TRE;

void TestStackAlloc();
void TestAlignedStackAlloc();
void TestLinearAlloc();
void TestPoolAlloc();

int main()
{
	/*TRE::BasicString<char> str("Hello friend.");
	printf("%s | Length = %d\n", str.Buffer(), str.Length());
	str.Insert(5, " there");
	printf("%s | Length = %d\n", str.Buffer(), str.Length());*/
	String text("Hey");
	String text2("Hey");
	printf("%d \n", (text == String("hello")));
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