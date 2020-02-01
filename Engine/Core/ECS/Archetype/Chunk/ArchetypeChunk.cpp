#include "ArchetypeChunk.hpp"
#include <Core/ECS/World/World.hpp>
#include <Core/ECS/Archetype/Archetype.hpp>
#include <Core/ECS/Component/BaseComponent.hpp>
#include <Core/ECS/Entity/Entity.hpp>
#include <Core/ECS/Archetype/Archetype.hpp>

TRE_NS_START

ArchetypeChunk::ArchetypeChunk(Archetype* archetype, uint8* comp_buffer, uint32 shared_comp_index) :
	m_Archetype(archetype), m_NextChunk(NULL), m_ComponentBuffer(comp_buffer), 
	m_EntitiesCount(0)/*, m_SharedComponentIndex(shared_comp_index)*/
{
}

ArchetypeChunk::~ArchetypeChunk()
{
	for (auto& c : m_Archetype->GetTypesBufferMarker()) {
		ComponentDeleteFunction freefn = BaseComponent::GetTypeDeleteFunction(c.first); // Get delete func
		uint8* comp_buffer = this->GetComponentsBuffer() + c.second; // get the coomponent buffer
		uint32 size = (uint32) BaseComponent::GetTypeSize(c.first);

		for (uint32 i = 0; i < m_EntitiesCount; i++) {
			uint32 index = i * size; // get index in buffer
			freefn((BaseComponent*) &comp_buffer[index]);
		}
	}
}

uint8* ArchetypeChunk::GetComponentBuffer(ComponentTypeID id) const
{
	const Map<ComponentTypeID, uint32>& map = m_Archetype->GetTypesBufferMarker();
	return this->GetComponentsBuffer() + map[id];
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
	return (BaseComponent*) this->GetComponent(entity.m_InternalId, this->GetComponentBuffer(id), id);
}

BaseComponent* ArchetypeChunk::GetComponent(EntityID internal_id, uint8* buffer, ComponentTypeID component_id) const
{
	return (BaseComponent*)(buffer + BaseComponent::GetTypeSize(component_id) * internal_id);
}

Entity* ArchetypeChunk::GetEntity(uint32 index)
{
	if (index >= m_EntitiesCount)
		return NULL;
	
	return &m_Archetype->GetEntityManager().GetEntityByID(GetEntityID(index));
}

ArchetypeChunk* ArchetypeChunk::SwapEntityWithLastOne(uint32 entity_internal_id)
{
	//  Check last chunk and replace with it
	ArchetypeChunk* last_chunk = m_Archetype->GetLastOccupiedChunk();

	if (!(entity_internal_id == last_chunk->m_EntitiesCount - 1 && last_chunk == this)) {
		// Update the internal id of the entity we are swapping with !
		Entity* last_entity = last_chunk->GetEntity(last_chunk->m_EntitiesCount - 1);

		if (last_entity) {
			((EntityID*)m_ComponentBuffer)[last_entity->m_InternalId] = 0;
			((EntityID*)m_ComponentBuffer)[entity_internal_id] = last_entity->m_Id;
			last_entity->m_InternalId = entity_internal_id;
			last_entity->m_Chunk = this;
		}
	}

	if (!--last_chunk->m_EntitiesCount) {
		// last_chunk->m_SharedComponentIndex = -1;
		m_Archetype->PushFreeChunk(last_chunk);
	}

	return last_chunk;
}

uint32 ArchetypeChunk::RemoveEntityComponents(uint32 entity_internal_id)
{
	ArchetypeChunk* last_chunk = SwapEntityWithLastOne(entity_internal_id);

	for (auto& buff : m_Archetype->GetTypesBufferMarker()) {
		this->RemoveComponentInternal(last_chunk, buff.first, this->GetComponentsBuffer() + buff.second, entity_internal_id);
	}

	return 1;
}

uint32 ArchetypeChunk::DestroyEntityComponents(uint32 entity_internal_id)
{
	ArchetypeChunk* last_chunk = SwapEntityWithLastOne(entity_internal_id);

	for (auto& buff : m_Archetype->GetTypesBufferMarker()) {
		this->DestroyComponentInternal(last_chunk, buff.first, this->GetComponentsBuffer() + buff.second, entity_internal_id);
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

	usize srcIndex = size * last_chunk->m_EntitiesCount;
	BaseComponent* srcComponent = (BaseComponent*) &(last_chunk->GetComponentBuffer(type_id)[srcIndex]);
	std::memcpy(destComponent, srcComponent, size);
}

void ArchetypeChunk::RemoveComponentInternal(ArchetypeChunk* last_chunk, ComponentTypeID type_id, uint8* components_buffer, EntityID internal_id)
{
	uint32 size = BaseComponent::GetTypeSize(type_id);
	BaseComponent* destComponent = (BaseComponent*) &components_buffer[internal_id * size];

	if (m_EntitiesCount > 1)
		return;

	usize srcIndex = size * last_chunk->m_EntitiesCount;
	BaseComponent* srcComponent = (BaseComponent*) &(last_chunk->GetComponentBuffer(type_id)[srcIndex]);
	std::memcpy(destComponent, srcComponent, size);
}

BaseComponent* ArchetypeChunk::UpdateComponentMemoryInternal(uint32 internal_id, Entity& entity, BaseComponent* component, ComponentTypeID component_id)
{
	ComponentCreateFunction createfn = BaseComponent::GetTypeCreateFunction(component_id);
	ComponentDeleteFunction freefn = BaseComponent::GetTypeDeleteFunction(component_id);
	BaseComponent* old_component = (BaseComponent*) (this->GetComponentBuffer(component_id) + internal_id * BaseComponent::GetTypeSize(component_id));
	freefn(old_component); // call dtor
	return createfn((uint8*)old_component, component);
}

BaseComponent* ArchetypeChunk::AddComponentToEntityInternal(Entity& entity, uint8* buffer, BaseComponent* component, ComponentTypeID component_id)
{
	ComponentCreateFunction createfn = BaseComponent::GetTypeCreateFunction(component_id);
	uint32 index = entity.m_InternalId * BaseComponent::GetTypeSize(component_id);
	return createfn((uint8*) &buffer[index], component);
}

void ArchetypeChunk::AddEntityComponents(Entity& entity, BaseComponent** components, const ComponentTypeID* componentIDs, usize numComponents)
{
	this->AddEntity(entity);

	for (uint32 i = 0; i < numComponents; i++) {
		const uint32& comp_id = componentIDs[i];
		AddComponentToEntityInternal(entity, this->GetComponentBuffer(comp_id), components[i], comp_id);
	}
}

uint32 ArchetypeChunk::ReserveEntity(const Entity& entity)
{
	((EntityID*)m_ComponentBuffer)[m_EntitiesCount] = entity.m_Id;
	return m_EntitiesCount++;
}

void ArchetypeChunk::AddEntity(Entity& entity)
{
	entity.AttachToArchetypeChunk(this, this->ReserveEntity(entity));
}

BaseComponent* ArchetypeChunk::UpdateComponentMemory(Entity& entity, BaseComponent* component, ComponentTypeID component_id)
{
	return this->UpdateComponentMemoryInternal(entity.m_InternalId, entity, component, component_id);
}

BaseComponent* ArchetypeChunk::AddComponentToEntity(Entity& entity, BaseComponent* component, ComponentTypeID component_id)
{
	return this->AddComponentToEntityInternal(entity, this->GetComponentBuffer(component_id), component, component_id);
}

uint32 ArchetypeChunk::RemoveEntityComponents(Entity& entity)
{
	entity.m_Chunk = NULL;
	return RemoveEntityComponents(entity.m_InternalId);
}

uint32 ArchetypeChunk::DestroyEntityComponents(Entity& entity)
{
	entity.m_Chunk = NULL;
	return DestroyEntityComponents(entity.m_InternalId);
}

EntityID& ArchetypeChunk::GetEntityID(uint32 internal_id) 
{ 
	return ((EntityID*)m_ComponentBuffer)[internal_id];
}

uint8* ArchetypeChunk::GetComponentsBuffer() const
{ 
	return m_ComponentBuffer + sizeof(EntityID) * ArchetypeChunk::CAPACITY;
}

Archetype& ArchetypeChunk::GetArchetype()
{ 
	return *m_Archetype;
}

TRE_NS_END