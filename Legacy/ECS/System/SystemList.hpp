#pragma once
#include <Legacy/Misc/Defines/Common.hpp>
#include <Legacy/DataStructure/Vector.hpp>
#include <Legacy/ECS/System/BaseSystem.hpp>

TRE_NS_START

class World;

class SystemList
{
public:
	enum SystemStatus
	{
		ACTIVE = 0,
		SLEEP = 1,
	};

	CONSTEXPR static uint32 NUM_LIST = 2;
public:
	SystemList(World* world);

	bool RemoveSystem(BaseSystem& system);

	bool AddSystem(BaseSystem* system);

	template<typename... Systems>
	void AddSystems(Systems*... sys);

	FORCEINLINE usize GetSize() { return m_Systems.Size(); }

	FORCEINLINE BaseSystem* operator[](uint32 index) { return m_Systems[index]; }
private:
	Vector<BaseSystem*> m_Systems;
	World* m_World;

	friend class BaseSystem;
};

template<typename... Systems>
void SystemList::AddSystems(Systems*... sys)
{
	CONSTEXPR usize num_sys = sizeof...(sys);
	BaseSystem* sys_arr[num_sys] = { (BaseSystem*)(sys)... };

	for (uint32 i = 0; i < num_sys; i++)
		this->AddSystem(sys_arr[i]);
}

TRE_NS_END
