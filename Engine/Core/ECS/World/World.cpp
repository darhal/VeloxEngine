#include "World.hpp"
#include <Core/DataStructure/Utils/Utils.hpp>
#include <Core/ECS/ECS/ECS.hpp>
#include <Core/ECS/System/BaseSystem.hpp>
#include <Core/ECS/Archetype/Chunk/ArchetypeChunk.hpp>

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
	}
}

TRE_NS_END