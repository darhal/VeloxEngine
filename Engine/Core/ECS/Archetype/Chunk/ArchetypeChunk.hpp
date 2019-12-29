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
	CONSTEXPR static uint32 CAPACITY = 10; // How much components per type can store
	typedef Map<ComponentTypeID, uint8*, LinearAllocator> TypeBufferMap;
public:
	ArchetypeChunk(Archetype* archetype, const LinearAllocator& map_alloc);

	~ArchetypeChunk();

	void AddComponentBuffer(ComponentTypeID id, uint8* start_of_buff);

	ArchetypeChunk* GetNextChunk();

	void SetNextChunk(ArchetypeChunk* next);

	const uint8* GetComponentBuffer(ComponentTypeID id) const;

	uint32 ReserveEntity();

	void AddEntity(Entity& entity);

	Entity* GetEntity(uint32 index);

	uint32 RemoveEntityComponents(Entity& entity);

	uint32 DestroyEntityComponents(Entity& entity);

	void AddEntityComponents(Entity& entity, BaseComponent** components, const ComponentTypeID* componentIDs, usize numComponents);

	FORCEINLINE BaseComponent* AddComponentToEntity(Entity& entity, BaseComponent* component, ComponentTypeID component_id);

	FORCEINLINE BaseComponent* UpdateComponentMemory(Entity& entity, BaseComponent* component, ComponentTypeID component_id);

	FORCEINLINE bool IsFull() const { return m_EntitiesCount == CAPACITY; };

	FORCEINLINE Archetype& GetArchetype() { return *m_Archetype; }

	BaseComponent* GetComponent(const Entity& entity, ComponentTypeID id) const;

	FORCEINLINE const TypeBufferMap& GetBufferMap() const { return m_StartMarkers; };

	BaseComponent* GetComponent(EntityID internal_id, uint8* buffer, ComponentTypeID component_id) const;
private:
	TypeBufferMap m_StartMarkers;
	ArchetypeChunk* m_NextChunk;
	Archetype* m_Archetype;
	uint32 m_EntitiesCount;

	void DestroyComponentInternal(ArchetypeChunk* last_chunk, ComponentTypeID type_id, uint8* components_buffer, EntityID internal_id);
	void RemoveComponentInternal(ArchetypeChunk* last_chunk, ComponentTypeID type_id, uint8* components_buffer, EntityID internal_id);
	BaseComponent* AddComponentToEntityInternal(Entity& entity, uint8* buffer, BaseComponent* component, ComponentTypeID component_id);
	BaseComponent* UpdateComponentMemoryInternal(uint32 internal_id, Entity& entity, BaseComponent* component, ComponentTypeID component_id);
};

FORCEINLINE BaseComponent* ArchetypeChunk::UpdateComponentMemory(Entity& entity, BaseComponent* component, ComponentTypeID component_id)
{
	return this->UpdateComponentMemoryInternal(entity.m_InternalId, entity, component, component_id);
}

FORCEINLINE BaseComponent* ArchetypeChunk::AddComponentToEntity(Entity& entity, BaseComponent* component, ComponentTypeID component_id)
{
	return this->AddComponentToEntityInternal(entity, m_StartMarkers[component_id], component, component_id);
}

TRE_NS_END