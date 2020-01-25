#ifdef BLABLA

#include <iostream>
#include <chrono>
#include <thread>
#include <future>
#include <time.h>
#include <unordered_map>

#include <Core/Context/Extensions.hpp>
#include <Core/Window/Window.hpp>
#include <Core/Misc/Maths/Maths.hpp>
#include <Core/Misc/Utils/Image.hpp>
#include <Core/Context/Context.hpp>
#include <Core/FileSystem/Directory/Directory.hpp>
#include <Core/DataStructure/HashMap/HashMap.hpp>
#include <Core/TaskSystem/TaskManager/TaskManager.hpp>
#include <Core/DataStructure/PackedArray/PackedArray.hpp>

#include <RenderAPI/Shader/ShaderProgram.hpp>
#include <RenderAPI/Shader/Shader.hpp>
#include <RenderAPI/VertexArray/VAO.hpp>
#include <RenderAPI/VertexBuffer/VBO.hpp>
#include <RenderAPI/Texture/Texture.hpp>
#include <RenderAPI/General/GLContext.hpp>
#include <RenderAPI/RenderBuffer/RBO.hpp>
#include <RenderAPI/FrameBuffer/FBO.hpp>
#include <RenderAPI/GlobalState/GLState.hpp>


#include <Core/DataStructure/Tuple/Tuple.hpp>
#include "ShaderValidator.hpp"
#include <Core/DataStructure/StringBuffer/StringBuffer.hpp>
#include <Core/DataStructure/FixedString/FixedString.hpp>

#include <Core/ECS/ECS/ECS.hpp>
#include <Core/DataStructure/Bitset/Bitset.hpp>
#include <Core/DataStructure/Utils/Utils.hpp>

using namespace TRE;


void output(int x)
{
	printf("X is %d\n", x);
}

void empty_job1(TaskExecutor* te, Job* t, const void*)
{
	printf("Hello I'm empty_job0 ! From Thread %d\n", te->GetWorkerID());
}

void empty_job2(TaskExecutor* te, Job* t, const void*)
{
	printf("Hello I'm empty_job1 ! From Thread %d\n", te->GetWorkerID());
}

void parallelJob1(TaskExecutor* te, Job* j, uint32* data, uint32 count)
{
	printf("Hey Im from Thread : %d\n", te->GetWorkerID());
	for (uint32 i = 0; i < count; i++) {
		printf("\tHey I have %d\n", data[i]);
	}
	printf("End of message\n");
}


void continuation(TaskExecutor* te, Job* t, const void*)
{
	printf("should after the first (continuation)\n");
}

void DoJobs1(TaskExecutor* ts)
{
	/*Task* root = ts->CreateTask(&empty_job1);
	ts->Run(root);
	for (usize i = 0; i < 5; i++) {
		Task* t = ts->CreateTask(&empty_job1);
		ts->AddContinuation(t, ts->CreateTask(continuation));
		ts->Run(t);
	}
	ts->Wait(root);*/

	uint32 data[20];
	for (uint i = 0; i < 20; i++) {
		data[i] = i;
	}
	Task* root = ts->ParallelFor(data, 20, parallelJob1, CountSplitter(5));
	ts->Run(root);
	ts->Wait(root);
}

void DoJobs2(TaskExecutor* ts)
{
	/*Task* root = ts->CreateTask(&empty_job2);
	ts->Run(root);
	for (usize i = 0; i < 3; i++) {
		Task* t = ts->CreateTask(&empty_job2);
		ts->Run(t);
	}
	ts->Wait(root);*/
	ts->DoWork();
}

struct TestComponent : public Component<TestComponent>
{
	TestComponent(const vec3& id) : smthg(id) {}
	vec3 smthg;
};

struct TestComponent2 : public Component<TestComponent2>
{
	//TestComponent2(const Mat4f& str) : test(str) {}
	//Mat4f test;

	TestComponent2(const String& str) : test(str) {}
	String test;
};

struct TestComponent3 : public Component<TestComponent3>
{
	TestComponent3(const Mat4f& str) : test(str) {}
	Mat4f test;
};

class TestSystemA : public System<TestComponent, TestComponent2>
{
public:
	TestSystemA()
	{
	}

	void UpdateComponents(float delta, Archetype& archetype) final
	{
		//printf("System A [ComponentType : %d]\n", comp_type);
		//LOG::Write("Updating the componenet : %p", components);
		for (ArchetypeChunk& chunk : archetype) {
			for (TestComponent& comp : chunk.Iterator<TestComponent>()) {
				comp.smthg += vec3(1.f, 2.f, 3.f);
				LOG::Write("[System A] Updating the componenet : (%f, %f, %f)", comp.smthg.x, comp.smthg.y, comp.smthg.z);
			}

			for (const TestComponent2& comp : chunk.Iterator<TestComponent2>()) {
				// LOG::Write("[System A] Updating the componenet : %s", comp.test.Buffer());
			}
		}
	}
};

class TestSystemB : public System<TestComponent>
{
public:
	TestSystemB()
	{
	}

	void UpdateComponents(float delta, Archetype& archetype) final
	{
		for (const ArchetypeChunk& chunk : archetype) {
			for (TestComponent& comp : chunk.Iterator<TestComponent>()) {
				comp.smthg += vec3(3.f, 2.f, 1.f);
				LOG::Write("[System B] Updating the componenet : (%f, %f, %f)", comp.smthg.x, comp.smthg.y, comp.smthg.z);
			}
		}
	}
};

class TestSystemC : public System<TestComponent2>
{
public:
	TestSystemC()
	{
	}

	void UpdateComponents(float delta, Archetype& archetype) final
	{
		//LOG::Write("Updating the componenet : %p", components);
		for (const ArchetypeChunk& chunk : archetype) {
			for (TestComponent2& comp : chunk.Iterator<TestComponent2>()) {
				comp.test += ", There!";
				LOG::Write("[System C] Updating the componenet : %s", comp.test.Buffer());
			}
		}
	}
};

class EntityA : public Entity
{

};

#define INIT_BENCHMARK std::chrono::time_point<std::chrono::high_resolution_clock> start, end; std::chrono::microseconds duration;

#define BENCHMARK(name, bloc_of_code) \
	start = std::chrono::high_resolution_clock::now(); \
	bloc_of_code; \
	end = std::chrono::high_resolution_clock::now();\
	duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start); \
	std::cout << "\nExecution of " << name << " took : " << duration.count() << " microsecond(s)" << std::endl; \

int main()
{
	// Testing PackedArray:
	/*PackedArray<char> array;

	auto& p1 = array.Emplace('A');
	printf("P1 (ID:%d, CHAR: %c)\n", p1.first, p1.second);
	auto& p2 = array.Emplace('B');
	printf("P2 (ID:%d, CHAR: %c)\n", p2.first, p2.second);
	auto& p3 = array.Emplace('C');
	printf("P3 (ID:%d, CHAR: %c)\n", p3.first, p3.second);

	{
		auto& c1 = array.Get(p1.first);
		printf("P1 (ID:%d, CHAR: %c)\n", p1.first, c1);
		auto& c2 = array.Get(p2.first);
		printf("P2 (ID:%d, CHAR: %c)\n", p2.first, c2);
		auto& c3 = array.Get(p3.first);
		printf("P3 (ID:%d, CHAR: %c)\n", p3.first, c3);
	}

	printf("Removing p1.first\n"); array.Remove(p1.first);

	{
		auto& c2 = array.Get(p2.first);
		printf("P2 (ID:%d, CHAR: %c)\n", p2.first, c2);
		auto& c3 = array.Get(p3.first);
		printf("P3 (ID:%d, CHAR: %c)\n", p3.first, c3);
	}

	auto& p4 = array.Emplace('D');
	printf("P4 (ID:%d, CHAR: %c)\n", p4.first, p4.second);
	auto& p5 = array.Emplace('E');
	printf("P5 (ID:%d, CHAR: %c)\n", p5.first, p5.second);
	auto& p6 = array.Emplace('F');
	printf("P6 (ID:%d, CHAR: %c)\n", p6.first, p6.second);

	{
		auto c2 = array.Get(p2.first);
		printf("P2 (ID:%d, CHAR: %c)\n", p2.first, c2);
		auto c3 = array.Get(p3.first);
		printf("P3 (ID:%d, CHAR: %c)\n", p3.first, c3);

		auto c4 = array.Get(p4.first);
		printf("P4 (ID:%d, CHAR: %c)\n", p4.first, c4);
		auto c5 = array.Get(p5.first);
		printf("P5 (ID:%d, CHAR: %c)\n", p5.first, c5);
		auto c6 = array.Get(p6.first);
		printf("P6 (ID:%d, CHAR: %c)\n", p6.first, c6);
	}

	getchar();
	return 0;*/

	LOG::Write("- Hardware Threads 	: %d", std::thread::hardware_concurrency());
	World& world = ECS::CreateWorld();
	TestSystemA test_systemA; TestSystemB test_systemB; TestSystemC test_systemC;
	world.GetSystsemList().AddSystems(&test_systemA, &test_systemB, &test_systemC);

	EntityID* ent = (EntityID*) Allocate<EntityID>(1'000);
	for (int i = 0; i < 1'000; i++) {
		// ent[i] = ECS::CreateEntityWithComponents(TestComponent2("Hello")).GetEntityID();
		if (i % 3 == 1) {
			ent[i] = world.GetEntityManager().CreateEntityWithComponents(TestComponent2("Hello"), TestComponent(vec3())).GetEntityID();
		} else if (i % 3 == 2) {
			ent[i] = world.GetEntityManager().CreateEntityWithComponents(TestComponent(vec3())).GetEntityID();
		} else if (i % 3 == 0) {
			ent[i] = world.GetEntityManager().CreateEntityWithComponents(TestComponent2("Hey")).GetEntityID();
		}
	}

	/*ArchetypeChunkIterator<TestComponent2> itr = world.GetEntityManager().GetEntityByID(ent[1]).GetChunk()->Iterator<TestComponent2>();
	for (TestComponent2& c : itr) {
		printf("All components of the chunk : %s\n", c.test);
	}

	Bitset sig(BaseComponent::GetComponentsCount());
	sig.Set(TestComponent::ID, true);
	sig.Set(TestComponent2::ID, true);
	uint32 count = 0;

	for (const Archetype* arche : world.GetEntityManager().GetAllArchetypesThatInclude(sig))
	{
		
		for (ArchetypeChunk& chunk : *arche) {
			if (arche->Has<TestComponent2>()) {
				ArchetypeChunkIterator<TestComponent2> itr = chunk.Iterator<TestComponent2>();
				for (TestComponent2& c : itr) {
					count++;
				}
			}

			if (arche->Has<TestComponent>()) {
				ArchetypeChunkIterator<TestComponent> itr2 = chunk.Iterator<TestComponent>();
				for (TestComponent& c : itr2) {
					count++;
				}
			}
		}
	}

	printf("Count: %d\n", count);*/

	/*Entity& entity = ECS::CreateEntityWithComponents(TestComponent2("Hello there!"), TestComponent(5));
	//entity.CreateComponent<TestComponent2>("Hello there!");
	//entity.CreateComponent<TestComponent>(5);

	Entity& entity2 = ECS::CreateEntityWithComponents(TestComponent(8));
	// entity2.CreateComponent<TestComponent>(8);
	*/
	int c;
	do {
		INIT_BENCHMARK
		BENCHMARK("Updating all components", world.UpdateSystems(0.0););

		uint32 index;
		std::cout << "Choose option (in [1..7]): "; std::cin >> c;
		std::cout << "Choose option (0-1'000'000): "; std::cin >> index;
		EntityID id = ent[index];

		if (c == 1) {
			INIT_BENCHMARK
			BENCHMARK("Remove TestComponent", world.GetEntityManager().GetEntityByID(id).RemoveComponent<TestComponent>(););
		} else if(c == 2) {
			BENCHMARK("Remove TestComponent2", world.GetEntityManager().GetEntityByID(id).RemoveComponent<TestComponent2>(););
		} else if (c == 3) {
			BENCHMARK("Create TestComponent", world.GetEntityManager().GetEntityByID(id).CreateComponent<TestComponent>(vec3()););
		} else if (c == 4) {
			BENCHMARK("Create TestComponent2", world.GetEntityManager().GetEntityByID(id).CreateComponent<TestComponent2>("Mat4f()"););
		} else if (c == 5) {
			BENCHMARK("Create TestComponent2 & TestComponent", 
				world.GetEntityManager().GetEntityByID(id).CreateComponent<TestComponent2>("Mat4f()");
			world.GetEntityManager().GetEntityByID(id).CreateComponent<TestComponent>(vec3());
			);
		} else if (c == 6) {
			BENCHMARK("Delete an entity!",
				world.GetEntityManager().DeleteEntity(id);
			);
		} else if (c == 7) {
			BENCHMARK("Create an entity!",
				ent[index] = world.GetEntityManager().CreateEntityWithComponents(TestComponent2("New Entity!"), TestComponent(vec3())).GetEntityID();
			);
		}
	}while(c !=  0);
	
	// RenderThread();
	getchar();
}

#endif