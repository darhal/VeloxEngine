#include "SystemList.hpp"
#include <Core/ECS/World/World.hpp>


TRE_NS_START

SystemList::SystemList(World* world) : m_World(world)
{
}

bool SystemList::RemoveSystem(BaseSystem& system)
{
	auto& systems = m_Systems;
	usize systems_sz = systems.Size();

	for (uint32 i = 0; i < systems_sz; i++) {
		if (&system == systems[i]) {
			// systems.Erease(systems.begin() + i); // TODO: FIX THIS
		}
	}

	return false;
}

bool SystemList::AddSystem(BaseSystem* system)
{
	system->AddToList(this);
	m_Systems.EmplaceBack(system);

	return true;
}

TRE_NS_END