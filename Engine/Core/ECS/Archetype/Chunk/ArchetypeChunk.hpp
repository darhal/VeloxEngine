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
	ArchetypeChunk(Archetype* archetype, uint8* comp_buffer);

	~ArchetypeChunk();

	ArchetypeChunk* GetNextChunk();

	void SetNextChunk(ArchetypeChunk* next);

	void AddEntity(Entity& entity);

	Entity* GetEntity(uint32 index);

	uint32 RemoveEntityComponents(uint32 entity_internal_id);

	uint32 DestroyEntityComponents(uint32 entity_internal_id);

	void AddEntityComponents(Entity& entity, BaseComponent** components, const ComponentTypeID* componentIDs, usize numComponents);

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

private:
	Archetype* m_Archetype;
	ArchetypeChunk* m_NextChunk;
	uint8* m_ComponentBuffer;
	uint32 m_EntitiesCount;

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
	if (m_Archetype->Has(Component::ID))
		return ArchetypeChunkIterator<Component>{ (Component*) this->GetComponentBuffer(Component::ID), m_EntitiesCount };

	return ArchetypeChunkIterator<Component>{ NULL, 0 };
}

TRE_NS_END