#pragma once

#include <Core/Misc/Defines/Common.hpp>
#include <Core/DataStructure/Vector/Vector.hpp>
#include <Core/Memory/Allocators/LinearAlloc/LinearAllocator.hpp>
#include <Core/DataStructure/HashMap/Map.hpp>
#include <Core/ECS/Component/BaseComponent.hpp>
#include <Core/ECS/Entity/Entity.hpp>

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

	uint8* GetComponentBuffer(ComponentTypeID id) const;

	uint32 ReserveEntity();

	void AddEntity(Entity& entity);

	Entity* GetEntity(uint32 index);

	uint32 RemoveEntityComponents(uint32 entity_internal_id);

	uint32 DestroyEntityComponents(uint32 entity_internal_id);

	void AddEntityComponents(Entity& entity, BaseComponent** components, const ComponentTypeID* componentIDs, usize numComponents);

	BaseComponent* GetComponent(const Entity& entity, ComponentTypeID id) const;

	BaseComponent* GetComponent(EntityID internal_id, uint8* buffer, ComponentTypeID component_id) const;

	FORCEINLINE BaseComponent* AddComponentToEntity(Entity& entity, BaseComponent* component, ComponentTypeID component_id);

	FORCEINLINE BaseComponent* UpdateComponentMemory(Entity& entity, BaseComponent* component, ComponentTypeID component_id);

	FORCEINLINE bool IsFull() const { return m_EntitiesCount >= CAPACITY; };

	FORCEINLINE Archetype& GetArchetype() { return *m_Archetype; }

	FORCEINLINE uint8* GetComponentsBuffer() { return m_ComponentBuffer; }

	FORCEINLINE uint32 RemoveEntityComponents(Entity& entity) { return RemoveEntityComponents(entity.m_InternalId); }

	FORCEINLINE uint32 DestroyEntityComponents(Entity& entity) { return RemoveEntityComponents(entity.m_InternalId); }

	FORCEINLINE uint32 GetEntitiesCount() const { return m_EntitiesCount; }

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
};

FORCEINLINE BaseComponent* ArchetypeChunk::UpdateComponentMemory(Entity& entity, BaseComponent* component, ComponentTypeID component_id)
{
	return this->UpdateComponentMemoryInternal(entity.m_InternalId, entity, component, component_id);
}

FORCEINLINE BaseComponent* ArchetypeChunk::AddComponentToEntity(Entity& entity, BaseComponent* component, ComponentTypeID component_id)
{
	return this->AddComponentToEntityInternal(entity, this->GetComponentBuffer(component_id), component, component_id);
}

TRE_NS_END