#include "ArchetypeChunk.hpp"

TRE_NS_START

ArchetypeChunk::ArchetypeChunk() : m_NextChunk(NULL), m_StartMarkers(), m_EntitiesCount(0)
{
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

const uint8* ArchetypeChunk::GetComponentBuffer(ComponentTypeID id) const
{
	return m_StartMarkers[id];
}

Entity* ArchetypeChunk::GetEntity(uint32 index)
{
	// TODO:
}

BaseComponent* ArchetypeChunk::AddComponentToEntityInternal(Entity& entity, uint8* buffer, BaseComponent* component, ComponentTypeID component_id)
{
	ComponentCreateFunction createfn = BaseComponent::GetTypeCreateFunction(component_id);
	uint32 index = m_EntitiesCount * BaseComponent::GetTypeSize(component_id);
	//return createfn(buffer, index, &entity, component);
}

uint32 ArchetypeChunk::RemoveEntityComponents(Entity& entity)
{

}

uint32 ArchetypeChunk::DestroyEntityComponents(Entity& entity)
{
	for (auto& buff : m_StartMarkers) {
		this->RemoveComponentInternal(buff.first, buff.second, entity.m_InternalId);
	}

	// TODO:
}

void ArchetypeChunk::DestroyComponentInternal(ComponentTypeID type_id, uint8* components_buffer, EntityID internal_id)
{
	uint32 size = BaseComponent::GetTypeSize(type_id);
	ComponentDeleteFunction freefn = BaseComponent::GetTypeDeleteFunction(type_id);
	ssize srcIndex = (ssize)(components_buffer + size * (m_EntitiesCount - 1));
	ssize destIndex = internal_id * size;
	BaseComponent* destComponent = (BaseComponent*)&components_buffer[destIndex];
	freefn(destComponent);

	if (srcIndex == destIndex)
		return;

	BaseComponent* srcComponent = (BaseComponent*)&components_buffer[srcIndex];
	std::memcpy(destComponent, srcComponent, size);
}

void ArchetypeChunk::RemoveComponentInternal(ComponentTypeID type_id, uint8* components_buffer, EntityID internal_id)
{
	uint32 size = BaseComponent::GetTypeSize(type_id);
	ssize srcIndex = (ssize)(components_buffer + size * (m_EntitiesCount - 1));
	ssize destIndex = internal_id * size;
	BaseComponent* destComponent = (BaseComponent*)&components_buffer[destIndex];

	if (srcIndex == destIndex)
		return;

	BaseComponent* srcComponent = (BaseComponent*)&components_buffer[srcIndex];
	std::memcpy(destComponent, srcComponent, size);
}

uint32 ArchetypeChunk::AddEntityComponents(Entity& entity, BaseComponent** components, const ComponentTypeID* componentIDs, usize numComponents)
{
	for (uint32 i = 0; i < numComponents; i++) {
		const uint32& comp_id = componentIDs[i];

		if (!BaseComponent::IsValidTypeID(comp_id)) {
			return -1;
		}

		AddComponentToEntityInternal(entity, m_StartMarkers[comp_id], components[i], comp_id);
	}

	// return entity.AttachToArchetypeChunk(*this, m_EntitiesCount++);
}

uint32 ArchetypeChunk::ReserveEntity()
{
	return m_EntitiesCount++;
}

TRE_NS_END