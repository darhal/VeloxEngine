#pragma once
#include <Core/Misc/Defines/Common.hpp>
#include <Core/DataStructure/Vector/Vector.hpp>
#include <Core/ECS/System/BaseSystem.hpp>

TRE_NS_START

class World;

class SystemList
{
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