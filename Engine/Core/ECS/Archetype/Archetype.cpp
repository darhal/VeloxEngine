#include "Archetype.hpp"

TRE_NS_START

uint32 Archetype::AddEntityComponents(IEntity& entity, BaseComponent** components, const ComponentTypeID* componentIDs, usize numComponents)
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

IEntity* Archetype::GetEntity(uint32 index)
{
	for (auto& buff : m_ComponentsBuffer) {
		uint32 size = BaseComponent::GetTypeSize(buff.first);
		BaseComponent* srcComponent = (BaseComponent*)&buff.second[index * size];
		return srcComponent->GetEntity();
	}
}

uint32 Archetype::RemoveEntityComponents(IEntity& entity)
{
	if (m_EntitiesCount > 1) {
		// update the internal id of the entity we are swapping with !
		this->GetEntity(m_EntitiesCount - 1)->m_InternalId = entity.m_InternalId;
	}

	return this->RemoveEntityComponentsInternal(entity.m_InternalId);
}

uint32 Archetype::RemoveEntityComponentsInternal(uint32 index)
{
	for (auto& buff : m_ComponentsBuffer) {
		this->RemoveComponent(buff.second, buff.first, index);
	}

	return --m_EntitiesCount;
}

void Archetype::RemoveComponent(Vector<uint8>& components_buffer, ComponentTypeID id, uint32 mem_index)
{
	ComponentDeleteFunction freefn = BaseComponent::GetTypeDeleteFunction(id);
	uint32 size = BaseComponent::GetTypeSize(id);
	ssize srcIndex = (ssize)(components_buffer.Size() - size);
	BaseComponent* destComponent = (BaseComponent*)&components_buffer[mem_index];
	freefn(destComponent);
	
	if (srcIndex == mem_index) {
		components_buffer.Resize(srcIndex);
		return;
	}
		
	components_buffer.Resize(srcIndex);
	BaseComponent* srcComponent = (BaseComponent*) &components_buffer[srcIndex];
	std::memcpy(destComponent, srcComponent, size);
}

uint32 Archetype::ReseveEntity()
{
	for (auto& ids : m_ComponentsBuffer) {
		Vector<uint8>& components_buffer = m_ComponentsBuffer[ids.first];
		components_buffer.Resize(components_buffer.Size() + BaseComponent::GetTypeSize(ids.first));
	}

	return m_EntitiesCount++;
}

BaseComponent* Archetype::UpdateComponentMemoryInternal(uint32 internal_id, IEntity& entity, BaseComponent* component, ComponentTypeID component_id)
{
	Vector<uint8>& components_buffer = m_ComponentsBuffer[component_id];
	ComponentCreateFunction createfn = BaseComponent::GetTypeCreateFunction(component_id);
	return createfn(components_buffer, internal_id * BaseComponent::GetTypeSize(component_id), &entity, component);
}

BaseComponent* Archetype::AddComponentToEntityInternal(IEntity& entity, Vector<uint8>& buffer, BaseComponent* component, ComponentTypeID component_id)
{
	ComponentCreateFunction createfn = BaseComponent::GetTypeCreateFunction(component_id);
	uint32 index = (uint32)buffer.Size();
	buffer.Resize(index + BaseComponent::GetTypeSize(component_id));
	return createfn(buffer, index, &entity, component);
}

TRE_NS_END