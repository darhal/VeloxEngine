#pragma once

#include "Core/Misc/Defines/Common.hpp"
#include <Core/ECS/Component/BaseComponent.hpp>
#include <Core/DataStructure/Vector/Vector.hpp>
#include <Core/DataStructure/Tuple/Pair.hpp>

TRE_NS_START

typedef uint32 EntityID;

class Archetype;
class ArchetypeChunk;

class Entity
{
public:
	Entity(EntityID id) : m_Chunk(NULL), m_Id(id), m_InternalId(-1)
	{}

	typedef uint32 IndexInMemory;

	virtual ~Entity() = default;

	FORCEINLINE EntityID GetEntityID() const { return this->m_Id; }

	FORCEINLINE ArchetypeChunk* GetChunk() const { return this->m_Chunk; }

	template<typename Component>
	FORCEINLINE Component* AddComponent(Component& comp);

	template<typename Component, typename... Args>
	FORCEINLINE Component* CreateComponent(Args&&... args);

	template<typename Component>
	FORCEINLINE bool RemoveComponent();

	template<typename Component>
	FORCEINLINE BaseComponent* GetComponent();
protected:
	ArchetypeChunk* m_Chunk;
	EntityID m_Id;
	EntityID m_InternalId;
	
	friend class ECS;
	friend class BaseSystem;
	friend class Archetype;
	friend class ArchetypeChunk;
private:
	uint32 AttachToArchetypeChunk(ArchetypeChunk* archetype_chunk, EntityID internal_id);
};

template<typename Component>
FORCEINLINE Component* Entity::AddComponent(Component& comp)
{
	return ECS::AddComponent(*this, &comp);
}

template<typename Component, typename... Args>
FORCEINLINE Component* Entity::CreateComponent(Args&&... args)
{
	Component component(std::forward<Args>(args)...);
	return ECS::AddComponent(*this, &component);
}

template<typename Component>
FORCEINLINE bool Entity::RemoveComponent()
{
	return ECS::RemoveComponent<Component>(*this);
}

template<typename Component>
FORCEINLINE BaseComponent* Entity::GetComponent()
{
	return ECS::GetComponent<Component>(*this);
}

TRE_NS_END