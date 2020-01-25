#include "SystemList.hpp"
#include <Core/ECS/World/World.hpp>


TRE_NS_START

SystemList::SystemList(World* world) : m_World(world), m_ActiveSystemsCount(0)
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

bool SystemList::AddSystem(BaseSystem* system, SystemStatus status)
{
	system->SetArchetype(&m_World->GetEntityManager().GetOrCreateArchetype(system->GetSignature()));
	usize sz = m_Systems.Size();
	m_Systems.EmplaceBack(system);
	
	if (status == ACTIVE) {
		if (sz - m_ActiveSystemsCount != 0) {
			m_Systems[sz] = m_Systems[m_ActiveSystemsCount]; // last system = first system which is sleeping
			m_Systems[m_ActiveSystemsCount] = system;
		}

		m_ActiveSystemsCount++;
	}

	return true;
}

void SystemList::SetSystemStatus(BaseSystem* system, SystemStatus status)
{
	usize sz = m_Systems.Size();
	uint32 id = 0;

	for (; id < sz; id++) {
		if (m_Systems[id] == system) {
			break;
		}
	}

	if (status == ACTIVE && id >= m_ActiveSystemsCount) {
		if (id != m_ActiveSystemsCount) {
			m_Systems[id] = m_Systems[m_ActiveSystemsCount];
			m_Systems[m_ActiveSystemsCount] = system;
		}

		m_ActiveSystemsCount++;
	} else if (status == SLEEP && id < m_ActiveSystemsCount) {
		if (id != m_ActiveSystemsCount) {
			m_Systems[id] = m_Systems[m_ActiveSystemsCount - 1];
			m_Systems[m_ActiveSystemsCount - 1] = system;
		}

		m_ActiveSystemsCount--;
	}
}

TRE_NS_END