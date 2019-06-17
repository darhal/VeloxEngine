#ifdef ABRA

#include <Core/DataStructure/String/String.hpp>
#include <Core/DataStructure/Array/Array.hpp>
#include <Core/Memory/Common.hpp>
#include <Core/Memory/RefCounter/RefCounter.hpp>
#include <Core/Memory/SmartPtr/SharedPointer.hpp>
#include <Core/DataStructure/Vector/Vector.hpp>
#include <Core/Memory/Allocators/PoolAlloc/PoolAllocator.hpp>
#include <Core/DataStructure/LinkedList/List/List.hpp>
#include <Core/Memory/Allocators/PoolAlloc/MultiPoolAllocator.hpp>
#include <Core/DataStructure/Tuple/Pair.hpp>
#include <Core/DataStructure/Tuple/Tuple.hpp>
#include <Core/DataStructure/BinaryTrees/BinaryTree.hpp>
#include <Core/DataStructure/BinaryTrees/RedBlackTree.hpp>
#include <Core/DataStructure/BinaryTrees/BST.hpp>
#include <Core/DataStructure/BinaryTrees/AVL.hpp>
#include <Core/DataStructure/BinaryTrees/RBT.hpp>
#include <Core/DataStructure/HashMap/HashMap.hpp>
#include <Core/DataStructure/HashMap/Map.hpp>
#include <Core/DataStructure/Stack/Stack.hpp>
#include <Core/DataStructure/Queue/Queue.hpp>
#include <Core/DataStructure/LinkedList/SingleList/SingleList.hpp>
#include <Windows.h>
#include <iostream>
#include <vector>
#include <string>
#include <sstream>
#include <algorithm>
#include <random>
#include <iostream>
#include <cstdio>
#include <stdlib.h>
#include <chrono>
#include <bitset>
#include <array>
#include <map>
#include <unordered_map>
#include <vector>
#include <list>

using namespace TRE;

void TestStackAlloc();
void TestAlignedStackAlloc();
void TestLinearAlloc();
void TestPoolAlloc();
void BenchmarkStdString();
void BenchmarkString();
void BenchmarkList();
void BenchmarkVector();
void BenchmarkMap();

class A
{
public:
	A() { printf("ctor\n"); }
	A(uint32 x, uint32 y) : x(x), y(y) {};
	A(const A& a) { x = a.x; y = a.y; printf("COPY CTOR\n"); }
	A(A&& a) { x = a.x; y = a.y; printf("MOVE CTOR\n"); }

	A& operator=(const A& a) { x = a.x; y = a.y; printf("COPY ASSIGNEMENT\n"); return *this; }
	A& operator=(const A&& a) { x = a.x; y = a.y; printf("MOVE ASSIGNEMENT\n"); return *this; }
	//~A() { printf("dtor\n"); }
	uint32 x;
	uint32 y;
};

int main()
{
	/*Queue<int> queue;
	printf("Queue Top = {");
	for (usize i = 0; i < 10; i++) {
		queue.Emplace(i);
		printf(", %d", *queue.Front());
	}
	printf("}\n");
	printf("Queue Pop front = {");
	while (queue.Front()) {
		printf(", %d", *queue.Front());
		queue.Pop();
	}
	printf("}\n");

	printf("---------------------------------------\n");

	Stack<int> stack;
	printf("Stack Top = {");
	for (usize i = 0; i < 10; i++) {
		stack.Emplace(i);
		printf(", %d", *stack.Top());
	}
	printf("}\n");	
	printf("Stack Pop front = {");
	while (stack.Top()) {
		printf(", %d", *stack.Top());
		stack.Pop();
	}
	printf("}\n");*/

	SingleList<int> list;
	for (usize i = 0; i < 10; i++) {
		list.EmplaceFront(i);
	}
	for (auto i : list) {
		printf("Element : %d\n", i);
	}
	

	/*RBTree bst;
	bst.insert(8);
	bst.insert(18);
	bst.insert(5);
	bst.insert(15);
	bst.insert(17);
	bst.insert(25);
	bst.insert(40);
	bst.insert(80);
	bst.deleteNode(25);
	bst.prettyPrint();
	printf("++++++++++++++++++++++++++++++++++++++++++\n");
	RBT<int, int> test;
	test.Insert(8, 8);
	test.Insert(18, 18);
	test.Insert(5, 5);
	test.Insert(15, 15);
	test.Insert(17, 17);
	test.Insert(25, 25);
	test.Insert(40, 40);
	test.Insert(80, 80);
	test.Remove(25);
	test.Print();*/
	
	/*Pair<int, A> pair(5,  A(5, 6));
	printf("Pair : first : %d | second : %d\n", pair.first, pair.second.x+pair.second.y);

	Tuple<int, A, String> tuple(5, A(5, 6), String("Hello"));
	printf("Tuple : first : %d | second : %d | third : %s\n", tuple.Get<0>(), tuple.Get<1>().x + tuple.Get<1>().y, tuple.Get<2>().Buffer());*/

	/*String str2 = res.second;
	printf("Content = %s | Capacity = %d | Len = %d | IsSmall = %s\n", str2.Buffer(), str2.Capacity(), str2.Length(), str2.IsSmall() ? "True" : "False");*/
	
	/*BST<int> tree(0);
	BST<int>::BTLeaf* cur	    = NULL;
	BST<int>::BTLeaf* left_cur  = NULL;
	BST<int>::BTLeaf* right_cur = NULL;
	Vector<int> vec(10);
	for (usize i = 0; i < 10; i++) {
		int r = rand() % 100;
		vec.EmplaceBack(r);
		printf("Number = %d\n", r);
		tree.Insert(r);
	}
	std::cout << "Max = " << tree.FindMax() << std::endl;
	std::cout << "Min = " << tree.FindMin() << std::endl;
	std::cout << "Find(78) = " << tree.Find(78) << std::endl;
	tree.Dump();
	for (int x : vec) {
		printf("Removing : %d\n", x);
		tree.Remove(x);	
		tree.Print();

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
		printf("[%d]=%d, ", i, vec.at(i));
	}
	printf("}\n");
	printf("--------------------------\n");
}

void BenchmarkVector()
{
const usize MAX = 1000;
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
	//PrintVec(arr);
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
	//PrintVec(arr);
}
}

void BenchmarkList()
{
	printf("*** Start of benchmark sizeof(List) = %d | sizeof(std::list) = %d ***\n", sizeof(List<String>), sizeof(std::list<String>));
	{
		auto start = std::chrono::steady_clock::now();
		List<String> test;
		auto end = std::chrono::steady_clock::now();
		auto diff = end - start;
		std::cout << "TRE::List<String> benchmark of declaration :" << std::chrono::duration<double, std::nano>(diff).count() << " ns" << std::endl;

		start = std::chrono::steady_clock::now();
		test.EmplaceBack("Hello!");
		test.EmplaceFront("Hey!");
		test.EmplaceBack("How are you ?");
		test.EmplaceBack("I'm fine!");
		test.EmplaceFront("Convo start.");
		test.EmplaceBack("Thats nice!.");
		test.EmplaceBack("Yes it is!.");
		test.EmplaceFront("The weather is good!.");
		test.EmplaceFront("I Agree!.");
		end = std::chrono::steady_clock::now();
		diff = end - start;
		std::cout << "TRE::List<String> benchmark multiple EmplaceBack and EmplaceFront Ops :" << std::chrono::duration<double, std::nano>(diff).count() << " ns" << std::endl;

		/*typename List<String>::Node* head = test.m_Head;
		while (head != NULL) {
			printf("%s\n", head->m_Obj.Buffer());
			head = head->m_Next;
		}
		printf("-------------------\n");*/
		start = std::chrono::steady_clock::now();
		test.PopBack();
		end = std::chrono::steady_clock::now();
		diff = end - start;
		std::cout << "TRE::List<String> benchmark of PopBack Op :" << std::chrono::duration<double, std::nano>(diff).count() << " ns" << std::endl;
		/*head = test.m_Head;
		while (head != NULL) {
			printf("%s\n", head->m_Obj.Buffer());
			head = head->m_Next;
		}*/

		start = std::chrono::steady_clock::now();
		test.PopFront();
		end = std::chrono::steady_clock::now();
		diff = end - start;
		std::cout << "TRE::List<String> benchmark of PopFront Op :" << std::chrono::duration<double, std::nano>(diff).count() << " ns" << std::endl;
		/*head = test.m_Head;
		while (head != NULL) {
			printf("%s\n", head->m_Obj.Buffer());
			head = head->m_Next;
		}*/
	}
	printf("-------------------\n");
	{
		auto start = std::chrono::steady_clock::now();
		std::list<String> test;
		auto end = std::chrono::steady_clock::now();
		auto diff = end - start;
		std::cout << "std::list<String> benchmark of declaration :" << std::chrono::duration<double, std::nano>(diff).count() << " ns" << std::endl;

		start = std::chrono::steady_clock::now();
		test.emplace_back("Hello!");
		test.emplace_front("Hey!");
		test.emplace_back("How are you ?");
		test.emplace_back("I'm fine!");
		test.emplace_front("Convo start.");
		test.emplace_back("Thats nice!.");
		test.emplace_back("Yes it is!.");
		test.emplace_front("The weather is good!.");
		test.emplace_front("I Agree!.");
		end = std::chrono::steady_clock::now();
		diff = end - start;
		std::cout << "std::list<String> benchmark multiple EmplaceBack and EmplaceFront Ops :" << std::chrono::duration<double, std::nano>(diff).count() << " ns" << std::endl;

		start = std::chrono::steady_clock::now();
		test.pop_back();
		end = std::chrono::steady_clock::now();
		diff = end - start;
		std::cout << "std::list<String> benchmark of PopBack Op :" << std::chrono::duration<double, std::nano>(diff).count() << " ns" << std::endl;

		start = std::chrono::steady_clock::now();
		test.pop_front();
		end = std::chrono::steady_clock::now();
		diff = end - start;
		std::cout << "std::list<String> benchmark of PopFront Op :" << std::chrono::duration<double, std::nano>(diff).count() << " ns" << std::endl;
	}
}

void BenchmarkHashMap()
{
	printf("sizeof(std::unordered_map<int, String>) = %d || sizeof(HashMap<int, String>) = %d\n", sizeof(std::unordered_map<int, String>), sizeof(HashMap<int, String>));
	{
		/*double avrg = 0.;
		for (usize i = 0; i < 1'000; i++) {
			auto start = std::chrono::steady_clock::now();
			HashMap<int, String> map;
			auto end = std::chrono::steady_clock::now();
			auto diff = end - start;
			avrg += std::chrono::duration<double, std::nano>(diff).count();
		}
		std::cout << "Benchmark of declaration of HashMap<int, String> x1000 :" << avrg/1'000 << " ns" << std::endl;*/

		auto start = std::chrono::steady_clock::now();
		HashMap<int, String> map;
		auto end = std::chrono::steady_clock::now();
		auto diff = end - start;
		std::cout << "Benchmark of declaration of HashMap<int, String> :" << std::chrono::duration<double, std::nano>(diff).count() << " ns" << std::endl;

		start = std::chrono::steady_clock::now();
		map.Put(5, String("Test"));
		end = std::chrono::steady_clock::now();
		diff = end - start;
		std::cout << "Benchmark of map.Put(5, Test)  :" << std::chrono::duration<double, std::nano>(diff).count() << " ns" << std::endl;

		//printf("Key = %d | Value = %s\n", pair.first, pair.second.Buffer());
		start = std::chrono::steady_clock::now();
		String* str = map.Get(5);
		end = std::chrono::steady_clock::now();
		diff = end - start;
		std::cout << "Benchmark of map.Get(5)  :" << std::chrono::duration<double, std::nano>(diff).count() << " ns" << std::endl;

		printf("Value for 5 = %s\n", str->Buffer());
	}
	printf("------------------------------------------------\n");
	{
		auto start = std::chrono::steady_clock::now();
		std::unordered_map<int, String> test;
		auto end = std::chrono::steady_clock::now();
		auto diff = end - start;
		std::cout << "Benchmark of declaration of std::unordered_map<int, String> :" << std::chrono::duration<double, std::nano>(diff).count() << " ns" << std::endl;

		start = std::chrono::steady_clock::now();
		test.emplace(5, String("Test"));
		end = std::chrono::steady_clock::now();
		diff = end - start;
		std::cout << "Benchmark of test[5] = Test  :" << std::chrono::duration<double, std::nano>(diff).count() << " ns" << std::endl;

		//printf("Key = %d | Value = %s\n", pair.first, pair.second.Buffer());
		start = std::chrono::steady_clock::now();
		String str = test[5];
		end = std::chrono::steady_clock::now();
		diff = end - start;
		std::cout << "Benchmark of test[5]  :" << std::chrono::duration<double, std::nano>(diff).count() << " ns" << std::endl;

		printf("Value for 5 = %s\n", str.Buffer());
	}
}

void BenchmarkMap()
{
	auto start = std::chrono::steady_clock::now();
	Map<int, String> map;
	auto end = std::chrono::steady_clock::now();
	auto diff = end - start;
	std::cout << "Declartion of Map<int, String> :" << std::chrono::duration<double, std::nano>(diff).count() << " ns" << std::endl;

	start = std::chrono::steady_clock::now();
	/*map.Emplace(5, "Hello");
	map.Emplace(1, "Thanks");
	map.Emplace(2, "Yes!");*/
	map[5] = "Hello";
	map[1] = "Thanks";
	map[2] = "Yes";
	end = std::chrono::steady_clock::now();
	diff = end - start;
	std::cout << "TRE::Map Multiple affectations:" << std::chrono::duration<double, std::nano>(diff).count() << " ns" << std::endl;

	start = std::chrono::steady_clock::now();
	auto res = map.Get(5);
	end = std::chrono::steady_clock::now();
	diff = end - start;
	std::cout << "TRE::Map get from key:" << std::chrono::duration<double, std::nano>(diff).count() << " ns" << std::endl;

	if (res != NULL)
		printf("Result found : %d | %s\n", 5, res->Buffer());
	start = std::chrono::steady_clock::now();
	res = map.Get(2);
	end = std::chrono::steady_clock::now();
	diff = end - start;
	std::cout << "TRE::Map get from key:" << std::chrono::duration<double, std::nano>(diff).count() << " ns" << std::endl;
	if (res != NULL)
		printf("Result found : %d | %s\n", 2, res->Buffer());
	start = std::chrono::steady_clock::now();
	res = map.Get(1);
	end = std::chrono::steady_clock::now();
	diff = end - start;
	std::cout << "TRE::Map get from key:" << std::chrono::duration<double, std::nano>(diff).count() << " ns" << std::endl;
	if (res != NULL)
		printf("Result found : %d | %s\n", 1, res->Buffer());
	start = std::chrono::steady_clock::now();
	res = map.Get(0);
	end = std::chrono::steady_clock::now();
	diff = end - start;
	std::cout << "TRE::Map get from key:" << std::chrono::duration<double, std::nano>(diff).count() << " ns" << std::endl;
	if (res != NULL)
		printf("Result found : %d | %s\n", 0, res->Buffer());

	printf("--------------------------------------------------------------\n");

	start = std::chrono::steady_clock::now();
	std::map<int, String> map2;
	end = std::chrono::steady_clock::now();
	diff = end - start;
	std::cout << "Declartion of std::map<int, String> :" << std::chrono::duration<double, std::nano>(diff).count() << " ns" << std::endl;

	start = std::chrono::steady_clock::now();
	/*map2.emplace(5, "Hello");
	map2.emplace(1, "Thanks");
	map2.emplace(2, "Yes");*/
	map2[5] = "Hello";
	map2[1] = "Thanks";
	map2[2] = "Yes";
	end = std::chrono::steady_clock::now();
	diff = end - start;
	std::cout << "std::map Multiple affectations:" << std::chrono::duration<double, std::nano>(diff).count() << " ns" << std::endl;

	start = std::chrono::steady_clock::now();
	auto str = map2[5];
	end = std::chrono::steady_clock::now();
	diff = end - start;
	std::cout << "STD::Map get from key:" << std::chrono::duration<double, std::nano>(diff).count() << " ns" << std::endl;

	if (res != NULL)
		printf("Result found : %d | %s\n", 5, str.Buffer());
	start = std::chrono::steady_clock::now();
	str = map2[2];
	end = std::chrono::steady_clock::now();
	diff = end - start;
	std::cout << "STD::Map get from key:" << std::chrono::duration<double, std::nano>(diff).count() << " ns" << std::endl;
	if (res != NULL)
		printf("Result found : %d | %s\n", 2, str.Buffer());
	start = std::chrono::steady_clock::now();
	str = map2[1];
	end = std::chrono::steady_clock::now();
	diff = end - start;
	std::cout << "STD::Map get from key:" << std::chrono::duration<double, std::nano>(diff).count() << " ns" << std::endl;
	if (res != NULL)
		printf("Result found : %d | %s\n", 1, str.Buffer());
	start = std::chrono::steady_clock::now();
	str = map2[0];
	end = std::chrono::steady_clock::now();
	diff = end - start;
	std::cout << "STD::Map get from key:" << std::chrono::duration<double, std::nano>(diff).count() << " ns" << std::endl;
	if (res != NULL)
		printf("Result found : %d | %s\n", 0, str.Buffer());
}

#endif