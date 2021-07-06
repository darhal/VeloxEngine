#pragma once

#include "Legacy/Misc/Defines/Common.hpp"
#include <Legacy/DataStructure/Vector.hpp>
#include <Legacy/DataStructure/Pair.hpp>
#include <Legacy/ECS/Common.hpp>
#include <Legacy/ECS/EntityManager/EntityManager.hpp>
#include <Legacy/ECS/Component/BaseComponent.hpp>
#include <Legacy/ECS/Archetype/Chunk/ArchetypeChunk.hpp>

TRE_NS_START

class Entity
{
public:
	typedef uint32 IndexInMemory;
public:
	Entity(EntityManager* manager, EntityID id) : m_Manager(manager), m_Chunk(NULL), m_Id(id), m_InternalId(-1)
	{}

	Entity(EntityManager* manager) : m_Manager(manager), m_Chunk(NULL), m_Id(-1), m_InternalId(-1)
	{}

	FORCEINLINE EntityID GetEntityID() const { return this->m_Id; }

	FORCEINLINE ArchetypeChunk* GetChunk() const { return this->m_Chunk; }

	FORCEINLINE Archetype* GetArchetype() const;

	template<typename Component>
	FORCEINLINE Component* AddComponent(Component* comp);

	template<typename Component, typename... Args>
	FORCEINLINE Component* CreateComponent(Args&&... args);

	template<typename Component>
	FORCEINLINE bool RemoveComponent();

	template<typename Component>
	FORCEINLINE Component* GetComponent();

	template<typename Component>
	bool HasComponent();
protected:
	EntityManager* m_Manager;
	ArchetypeChunk* m_Chunk;
	EntityID m_Id;
	EntityID m_InternalId;
	
	friend class EntityManager;
	friend class EntityContainer;
	friend class BaseSystem;
	friend class Archetype;
	friend class ArchetypeChunk;
private:
	uint32 AttachToArchetypeChunk(ArchetypeChunk* archetype_chunk, EntityID internal_id);
};

template<typename Component>
FORCEINLINE Component* Entity::AddComponent(Component* comp)
{
	return m_Manager->AddComponent(*this, comp);
}

template<typename Component, typename... Args>
FORCEINLINE Component* Entity::CreateComponent(Args&&... args)
{
	Component component(::std::forward<Args>(args)...);
	return m_Manager->AddComponent(*this, &component);
}

template<typename Component>
FORCEINLINE bool Entity::RemoveComponent()
{
	return m_Manager->RemoveComponent<Component>(*this);
}

template<typename Component>
FORCEINLINE Component* Entity::GetComponent()
{
	return m_Manager->GetComponent<Component>(*this);
}

FORCEINLINE Archetype* Entity::GetArchetype() const
{ 
	if (m_Chunk)
		return &m_Chunk->GetArchetype();

	return NULL;
}

template<typename Component>
bool Entity::HasComponent()
{
	return m_Manager->GetComponent<Component>(*this);
}

TRE_NS_END
