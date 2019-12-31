#pragma once

#include <type_traits>
#include <Core/Misc/Defines/Common.hpp>
#include <Core/ECS/EntityManager/EntityManager.hpp>
#include <Core/ECS/System/SystemList.hpp>

TRE_NS_START

class World
{
public:
	World();

	~World();

	const uint32& GetWorldID() const { return m_WorldId; }

	EntityManager& GetEntityManager() { return m_EntityManager; }

	SystemList& GetSystsemList() { return m_SystemList; }

	void UpdateSystems(float delta);
public:
	EntityManager m_EntityManager;
	SystemList m_SystemList;

private:
	uint32 m_WorldId;

	friend class ECS;
};



TRE_NS_END