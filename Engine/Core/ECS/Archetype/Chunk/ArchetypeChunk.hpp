#pragma once

#include <Core/Misc/Defines/Common.hpp>
#include <Core/DataStructure/Vector/Vector.hpp>
#include <Core/Memory/Allocators/LinearAlloc/LinearAllocator.hpp>
#include <Core/ECS/Common.hpp>
#include <Core/ECS/Archetype/Chunk/ArchetypeChunkIterator.hpp>
#include <Core/ECS/Archetype/Archetype.hpp>

TRE_NS_START

class ArchetypeChunk
{
public:
	CONSTEXPR static uint32 CAPACITY = 64; // How much components per type can store

public:
	ArchetypeChunk(Archetype* archetype, uint8* comp_buffer, uint32 shared_comp_index = -1);

	~ArchetypeChunk();

	ArchetypeChunk* GetNextChunk();

	void SetNextChunk(ArchetypeChunk* next);

	void AddEntity(Entity& entity);

	Entity* GetEntity(uint32 index);

	uint32 RemoveEntityComponents(uint32 entity_internal_id);

	uint32 DestroyEntityComponents(uint32 entity_internal_id);

	void AddEntityComponents(Entity& entity, BaseComponent** components, const ComponentTypeID* componentIDs, usize numComponents);

	//template<typename SharedComp>
	//SharedComp* GetSharedComponent() const;
	
	BaseComponent* GetComponent(const Entity& entity, ComponentTypeID id) const;

	BaseComponent* GetComponent(EntityID internal_id, uint8* buffer, ComponentTypeID component_id) const;

	BaseComponent* AddComponentToEntity(Entity& entity, BaseComponent* component, ComponentTypeID component_id);

	BaseComponent* UpdateComponentMemory(Entity& entity, BaseComponent* component, ComponentTypeID component_id);

	uint32 RemoveEntityComponents(Entity& entity);

	uint32 DestroyEntityComponents(Entity& entity);

	uint8* GetComponentBuffer(ComponentTypeID id) const;

	uint8* GetComponentsBuffer() const;

	Archetype& GetArchetype();

	FORCEINLINE bool IsFull() const { return m_EntitiesCount >= CAPACITY; };

	FORCEINLINE uint32 GetEntitiesCount() const { return m_EntitiesCount; }

	EntityID& GetEntityID(uint32 internal_id);

	template<typename Component>
	ArchetypeChunkIterator<Component> Iterator();

	template<typename Component>
	ArchetypeChunkIterator<Component> Iterator() const;

	FORCEINLINE ArchetypeChunkIterator<EntityID> EntityIterator();

	FORCEINLINE ArchetypeChunkIterator<EntityID> EntityIterator() const;

	template<typename Component>
	FORCEINLINE Component& GetComponentByInternalID(uint32 id);

	template<typename Component>
	FORCEINLINE Component& GetComponentByInternalID(uint32 id) const;
private:
	Archetype* m_Archetype;
	ArchetypeChunk* m_NextChunk;
	uint8* m_ComponentBuffer;
	uint32 m_EntitiesCount;
	// uint32 m_SharedComponentIndex;

	void DestroyComponentInternal(ArchetypeChunk* last_chunk, ComponentTypeID type_id, uint8* components_buffer, EntityID internal_id);

	void RemoveComponentInternal(ArchetypeChunk* last_chunk, ComponentTypeID type_id, uint8* components_buffer, EntityID internal_id);

	BaseComponent* AddComponentToEntityInternal(Entity& entity, uint8* buffer, BaseComponent* component, ComponentTypeID component_id);

	BaseComponent* UpdateComponentMemoryInternal(uint32 internal_id, Entity& entity, BaseComponent* component, ComponentTypeID component_id);

	ArchetypeChunk* SwapEntityWithLastOne(uint32 entity_internal_id);

	uint32 ReserveEntity(const Entity& entity);
};

template<typename Component>
ArchetypeChunkIterator<Component> ArchetypeChunk::Iterator()
{
	if (m_Archetype->HasComponentType(Component::ID))
		return ArchetypeChunkIterator<Component>{ (Component*) this->GetComponentBuffer(Component::ID), m_EntitiesCount };

	return ArchetypeChunkIterator<Component>{ NULL, 0 };
}

template<typename Component>
ArchetypeChunkIterator<Component> ArchetypeChunk::Iterator() const
{
	if (m_Archetype->HasComponentType(Component::ID))
		return ArchetypeChunkIterator<Component>{ (Component*)this->GetComponentBuffer(Component::ID), m_EntitiesCount };

	return ArchetypeChunkIterator<Component>{ NULL, 0 };
}

FORCEINLINE ArchetypeChunkIterator<EntityID> ArchetypeChunk::EntityIterator()
{
	if (m_Archetype->IsEmpty())
		return ArchetypeChunkIterator<EntityID>{ NULL, 0 };
	
	return ArchetypeChunkIterator<EntityID>{ (EntityID*)m_ComponentBuffer, m_EntitiesCount };
}

FORCEINLINE ArchetypeChunkIterator<EntityID> ArchetypeChunk::EntityIterator() const
{
	if (m_Archetype->IsEmpty())
		return ArchetypeChunkIterator<EntityID>{ NULL, 0 };

	return ArchetypeChunkIterator<EntityID>{ (EntityID*)m_ComponentBuffer, m_EntitiesCount };
}

template<typename Component>
FORCEINLINE Component& ArchetypeChunk::GetComponentByInternalID(uint32 id)
{
	ASSERTF(id >= m_EntitiesCount, "Invalid internal ID supplied to the chunk.");
	return *(Component*)(this->GetComponentBuffer(Component::ID) + BaseComponent::GetTypeSize(Component::ID) * id);
}

template<typename Component>
FORCEINLINE Component& ArchetypeChunk::GetComponentByInternalID(uint32 id) const
{
	ASSERTF(id >= m_EntitiesCount, "Invalid internal ID supplied to the chunk.");
	return *(Component*)(this->GetComponentBuffer(Component::ID) + BaseComponent::GetTypeSize(Component::ID) * id);
}

/*template<typename SharedComp>
SharedComp* ArchetypeChunk::GetSharedComponent() const
{
	if (m_SharedComponentIndex != uint32(-1)) {
		return m_Archetype->template GetSharedComponent<SharedComp>(m_SharedComponentIndex);
	}
}*/

TRE_NS_END