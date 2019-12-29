#include "ArchetypeChunk.hpp"
#include <Core/ECS/Archetype/Archetype.hpp>

TRE_NS_START

ArchetypeChunk::ArchetypeChunk(Archetype* archetype, const LinearAllocator& map_alloc) :
	m_StartMarkers(map_alloc), m_NextChunk(NULL), m_Archetype(archetype), m_EntitiesCount(0)
{
}

ArchetypeChunk::~ArchetypeChunk()
{
	for (uint32 i = 0; i < m_EntitiesCount; i++) {
		for (auto& c : m_StartMarkers) {
			ComponentDeleteFunction freefn = BaseComponent::GetTypeDeleteFunction(c.first);
			uint32 index = i * BaseComponent::GetTypeSize(c.first);
			freefn((BaseComponent*)c.second[index]);
		}
	}
}

void ArchetypeChunk::AddComponentBuffer(ComponentTypeID id, uint8* start_of_buff)
{
	m_StartMarkers.Emplace(id, start_of_buff);
}

ArchetypeChunk* ArchetypeChunk::GetNextChunk()
{
	return m_NextChunk;
}

void ArchetypeChunk::SetNextChunk(ArchetypeChunk* next)
{
	m_NextChunk = next;
}

BaseComponent* ArchetypeChunk::GetComponent(const Entity& entity, ComponentTypeID id) const
{
	return (BaseComponent*) this->GetComponent(entity.m_InternalId, m_StartMarkers[id], id);
}

BaseComponent* ArchetypeChunk::GetComponent(EntityID internal_id, uint8* buffer, ComponentTypeID component_id) const
{
	return (BaseComponent*)(buffer + BaseComponent::GetTypeSize(component_id) * internal_id);
}

const uint8* ArchetypeChunk::GetComponentBuffer(ComponentTypeID id) const
{
	return m_StartMarkers[id];
}

Entity* ArchetypeChunk::GetEntity(uint32 index)
{
	for (auto& buff : m_StartMarkers) {
		uint32 size = BaseComponent::GetTypeSize(buff.first);
		BaseComponent* srcComponent = (BaseComponent*) &buff.second[index * size];
		return srcComponent->GetEntity();
	}

	return NULL;
}

uint32 ArchetypeChunk::RemoveEntityComponents(Entity& entity)
{
	//  Check last chunk and replace with it
	ArchetypeChunk* last_chunk = m_Archetype->GetLastOccupiedChunk();

	if (m_EntitiesCount > 1) {
		// update the internal id of the entity we are swapping with !
		Entity* last_entity = last_chunk->GetEntity(m_EntitiesCount - 1);
		last_entity->m_InternalId = entity.m_InternalId;
		last_entity->m_Chunk = this;
		if (!--last_chunk->m_EntitiesCount) {
			m_Archetype->PushFreeChunk(last_chunk);
		}
	}

	for (auto& buff : m_StartMarkers) {
		this->RemoveComponentInternal(last_chunk, buff.first, buff.second, entity.m_InternalId);
	}

	return 1;
}

uint32 ArchetypeChunk::DestroyEntityComponents(Entity& entity)
{
	//  Check last chunk and replace with it
	ArchetypeChunk* last_chunk = m_Archetype->GetLastOccupiedChunk();

	if (m_EntitiesCount > 1) {
		// update the internal id of the entity we are swapping with !
		Entity* last_entity = last_chunk->GetEntity(m_EntitiesCount - 1);
		last_entity->m_InternalId = entity.m_InternalId;
		last_entity->m_Chunk = this;
		if (!--last_chunk->m_EntitiesCount) {
			m_Archetype->PushFreeChunk(last_chunk);
		}
	}

	for (auto& buff : m_StartMarkers) {
		this->DestroyComponentInternal(last_chunk, buff.first, buff.second, entity.m_InternalId);
	}

	return 1;
}

void ArchetypeChunk::DestroyComponentInternal(ArchetypeChunk* last_chunk, ComponentTypeID type_id, uint8* components_buffer, EntityID internal_id)
{
	uint32 size = BaseComponent::GetTypeSize(type_id);
	ComponentDeleteFunction freefn = BaseComponent::GetTypeDeleteFunction(type_id);
	BaseComponent* destComponent = (BaseComponent*) &components_buffer[internal_id * size];
	freefn(destComponent); // call dtor

	if (m_EntitiesCount > 1)
		return;

	usize srcIndex = (size * (last_chunk->m_EntitiesCount - 1));
	BaseComponent* srcComponent = (BaseComponent*) &(last_chunk->GetComponentBuffer(type_id)[srcIndex]);
	std::memcpy(destComponent, srcComponent, size);
}

void ArchetypeChunk::RemoveComponentInternal(ArchetypeChunk* last_chunk, ComponentTypeID type_id, uint8* components_buffer, EntityID internal_id)
{
	uint32 size = BaseComponent::GetTypeSize(type_id);
	BaseComponent* destComponent = (BaseComponent*)&components_buffer[internal_id * size];

	if (m_EntitiesCount > 1)
		return;

	usize srcIndex = (size * (last_chunk->m_EntitiesCount - 1));
	BaseComponent* srcComponent = (BaseComponent*) &(last_chunk->GetComponentBuffer(type_id)[srcIndex]);
	std::memcpy(destComponent, srcComponent, size);
}

BaseComponent* ArchetypeChunk::UpdateComponentMemoryInternal(uint32 internal_id, Entity& entity, BaseComponent* component, ComponentTypeID component_id)
{
	ComponentCreateFunction createfn = BaseComponent::GetTypeCreateFunction(component_id);
	ComponentDeleteFunction freefn = BaseComponent::GetTypeDeleteFunction(component_id);
	BaseComponent* old_component = (BaseComponent*) (m_StartMarkers[component_id] + internal_id * BaseComponent::GetTypeSize(component_id));
	freefn(old_component); // call dtor
	return createfn((uint8*)old_component, &entity, component);
}

BaseComponent* ArchetypeChunk::AddComponentToEntityInternal(Entity& entity, uint8* buffer, BaseComponent* component, ComponentTypeID component_id)
{
	ComponentCreateFunction createfn = BaseComponent::GetTypeCreateFunction(component_id);
	uint32 index = entity.m_InternalId * BaseComponent::GetTypeSize(component_id);
	return createfn(buffer + index, &entity, component);
}

void ArchetypeChunk::AddEntityComponents(Entity& entity, BaseComponent** components, const ComponentTypeID* componentIDs, usize numComponents)
{
	entity.AttachToArchetypeChunk(this, m_EntitiesCount++);

	for (uint32 i = 0; i < numComponents; i++) {
		const uint32& comp_id = componentIDs[i];
		AddComponentToEntityInternal(entity, m_StartMarkers[comp_id], components[i], comp_id);
	}
}

uint32 ArchetypeChunk::ReserveEntity()
{
	return m_EntitiesCount++;
}

void ArchetypeChunk::AddEntity(Entity& entity)
{
	entity.AttachToArchetypeChunk(this, this->ReserveEntity());
}

TRE_NS_END