#include "Archetype.hpp"

TRE_NS_START

uint32 Archetype::AddEntityComponents(Entity& entity, BaseComponent** components, const ComponentTypeID* componentIDs, usize numComponents)
{
	for (uint32 i = 0; i < numComponents; i++) {
		const uint32& comp_id = componentIDs[i];

		if (!BaseComponent::IsValidTypeID(comp_id)) {
			return -1;
		}

		AddComponentToEntityInternal(entity, m_ComponentsBuffer[comp_id], components[i], comp_id);
	}

	return entity.AttachToArchetype(*this, m_EntitiesCount++);
}

Entity* Archetype::GetEntity(uint32 index)
{
	for (auto& buff : m_ComponentsBuffer) {
		uint32 size = BaseComponent::GetTypeSize(buff.first);
		BaseComponent* srcComponent = (BaseComponent*)&buff.second[index * size];
		return srcComponent->GetEntity();
	}

	return NULL;
}

uint32 Archetype::RemoveEntityComponents(Entity& entity)
{
	Entity* last_entity;
	if (m_EntitiesCount > 1 /*&& (last_entity = this->GetEntity(m_EntitiesCount - 1)) != NULL*/) {
		// update the internal id of the entity we are swapping with !
		last_entity = this->GetEntity(m_EntitiesCount - 1);
		last_entity->m_InternalId = entity.m_InternalId;
	}

	return this->RemoveEntityComponentsInternal(entity.m_InternalId);
}

uint32 Archetype::RemoveEntityComponentsInternal(uint32 index)
{
	for (auto& buff : m_ComponentsBuffer) {
		this->RemoveComponentInternal(buff.second, buff.first, index);
	}

	return --m_EntitiesCount;
}

void Archetype::RemoveComponentInternal(Vector<uint8>& components_buffer, ComponentTypeID id, uint32 mem_index)
{
	uint32 size = BaseComponent::GetTypeSize(id);
	ssize srcIndex = (ssize)(components_buffer.Size() - size);
	ssize destIndex = mem_index * size;
	BaseComponent* destComponent = (BaseComponent*)&components_buffer[destIndex];
	
	if (srcIndex == destIndex) {
		components_buffer.Resize(srcIndex);
		return;
	}
		
	BaseComponent* srcComponent = (BaseComponent*) &components_buffer[srcIndex];
	std::memcpy(destComponent, srcComponent, size);
	components_buffer.Resize(srcIndex);
}

uint32 Archetype::DestroyEntityComponents(Entity& entity)
{
	if (m_EntitiesCount > 1) {
		// update the internal id of the entity we are swapping with !
		this->GetEntity(m_EntitiesCount - 1)->m_InternalId = entity.m_InternalId;
	}

	return this->RemoveEntityComponentsInternal(entity.m_InternalId);
}

uint32 Archetype::DestroyEntityComponentsInternal(uint32 index)
{
	for (auto& buff : m_ComponentsBuffer) {
		this->DestroyComponentInternal(buff.second, buff.first, index);
	}

	return --m_EntitiesCount;
}

void Archetype::DestroyComponentInternal(Vector<uint8>& components_buffer, ComponentTypeID id, uint32 mem_index)
{
	ComponentDeleteFunction freefn = BaseComponent::GetTypeDeleteFunction(id);
	uint32 size = BaseComponent::GetTypeSize(id);
	ssize srcIndex = (ssize)(components_buffer.Size() - size);
	ssize destIndex = mem_index * size;
	BaseComponent* destComponent = (BaseComponent*)&components_buffer[destIndex];
	freefn(destComponent);

	if (srcIndex == destIndex) {
		components_buffer.Resize(srcIndex);
		return;
	}

	BaseComponent* srcComponent = (BaseComponent*)&components_buffer[srcIndex];
	std::memcpy(destComponent, srcComponent, size);
	components_buffer.Resize(srcIndex);
}

void Archetype::DestroyComponent(Entity& entity, ComponentTypeID id)
{
	return this->DestroyComponentHelper(m_ComponentsBuffer[id], id, entity.m_InternalId);
}

void Archetype::DestroyComponentHelper(Vector<uint8>& components_buffer, ComponentTypeID id, uint32 mem_index)
{
	ComponentDeleteFunction freefn = BaseComponent::GetTypeDeleteFunction(id);
	uint32 size = BaseComponent::GetTypeSize(id);
	ssize destIndex = mem_index * size;
	BaseComponent* destComponent = (BaseComponent*)&components_buffer[destIndex];
	freefn(destComponent);
}

uint32 Archetype::ReseveEntity()
{
	for (auto& ids : m_ComponentsBuffer) {
		Vector<uint8>& components_buffer = ids.second;
		components_buffer.Resize(components_buffer.Size() + BaseComponent::GetTypeSize(ids.first));
		//printf("[RESERVE] Comp id : %d | Reserve : Old Size : %d | Neew Size : %d\n", ids.first, components_buffer.Size() - BaseComponent::GetTypeSize(ids.first), components_buffer.Size());
	}

	return m_EntitiesCount++;
}

BaseComponent* Archetype::UpdateComponentMemoryInternal(uint32 internal_id, Entity& entity, BaseComponent* component, ComponentTypeID component_id)
{
	Vector<uint8>& components_buffer = m_ComponentsBuffer[component_id];
	ComponentCreateFunction createfn = BaseComponent::GetTypeCreateFunction(component_id);
	//printf("Comp id : %d | Mem index : %d | Buffer Size = %d\n", component_id, internal_id * BaseComponent::GetTypeSize(component_id), components_buffer.Size());
	return createfn(components_buffer, internal_id * BaseComponent::GetTypeSize(component_id), &entity, component);
}

BaseComponent* Archetype::AddComponentToEntityInternal(Entity& entity, Vector<uint8>& buffer, BaseComponent* component, ComponentTypeID component_id)
{
	ComponentCreateFunction createfn = BaseComponent::GetTypeCreateFunction(component_id);
	uint32 index = (uint32)buffer.Size();
	buffer.Resize(index + BaseComponent::GetTypeSize(component_id));
	return createfn(buffer, index, &entity, component);
}

TRE_NS_END