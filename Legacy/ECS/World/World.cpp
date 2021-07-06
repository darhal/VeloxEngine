#include "World.hpp"

#include <Legacy/DataStructure/Utils.hpp>
#include <Legacy/ECS/ECS/ECS.hpp>
#include <Legacy/ECS/System/BaseSystem.hpp>
#include <Legacy/ECS/Archetype/Chunk/ArchetypeChunk.hpp>

TRE_NS_START

World::World() : 
	m_EntityManager(this), m_SystemList{this, this}, m_WorldId(ECS::DefaultWorld)
{
}

World::~World()
{
}

void World::UpdateSystems(float delta)
{
	SystemList& list = m_SystemList[SystemList::ACTIVE];
	usize systems_sz = list.GetSize();

	for (uint32 i = 0; i < systems_sz; i++) {
		BaseSystem& system = *list[i];
		
		system.OnUpdate(delta);
		system.FlushCommandsRecord();
	}
}

TRE_NS_END
