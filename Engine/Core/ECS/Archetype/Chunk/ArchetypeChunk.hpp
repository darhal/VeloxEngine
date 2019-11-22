#pragma once

#include <Core/Misc/Defines/Common.hpp>
#include <Core/DataStructure/Vector/Vector.hpp>
#include <Core/DataStructure/HashMap/Map.hpp>
#include <Core/ECS/Component/BaseComponent.hpp>
#include <Core/ECS/Entity/Entity.hpp>

TRE_NS_START

class ArchetypeChunk
{
public:
	ArchetypeChunk();

	void AddComponentBuffer(ComponentTypeID id, uint8* start_of_buff);

	ArchetypeChunk* GetNextChunk();

	void SetNextChunk(ArchetypeChunk* next);

	const uint8* GetComponentBuffer(ComponentTypeID id) const;

	uint32 AddEntityComponents(Entity& entity, BaseComponent** components, const ComponentTypeID* componentIDs, usize numComponents);

	uint32 ReserveEntity();

	uint32 RemoveEntityComponents(Entity& entity);

	uint32 DestroyEntityComponents(Entity& entity);

	Entity* GetEntity(uint32 index);
	
private:
	ArchetypeChunk* m_NextChunk;
	Map<ComponentTypeID, uint8*> m_StartMarkers;
	uint32 m_EntitiesCount;

	void DestroyComponentInternal(ComponentTypeID type_id, uint8* components_buffer, EntityID internal_id);
	void RemoveComponentInternal(ComponentTypeID type_id, uint8* components_buffer, EntityID internal_id);
	BaseComponent* AddComponentToEntityInternal(Entity& entity, uint8* buffer, BaseComponent* component, ComponentTypeID component_id);
};

TRE_NS_END