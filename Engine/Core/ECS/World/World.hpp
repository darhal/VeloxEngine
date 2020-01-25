#pragma once

#include <type_traits>
#include <Core/Misc/Defines/Common.hpp>
#include <Core/ECS/EntityManager/EntityManager.hpp>
#include <Core/ECS/System/SystemList.hpp>
#include <Core/ECS/Archetype/Archetype.hpp>
#include <Core/ECS/Archetype/Chunk/ArchetypeChunk.hpp>

TRE_NS_START

class World
{
public:
	World();

	~World();

	const uint32& GetWorldID() const { return m_WorldId; }

	EntityManager& GetEntityManager() { return m_EntityManager; }

	SystemList& GetSystsemList(SystemList::SystemStatus status) { return m_SystemList[status]; }

	void UpdateSystems(float delta);

public:
	EntityManager m_EntityManager;
	SystemList m_SystemList[SystemList::NUM_LIST];

private:
	
	uint32 m_WorldId;

	friend class ECS;
};



TRE_NS_END