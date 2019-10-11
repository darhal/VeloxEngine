#include "ECS.hpp"

TRE_NS_START

Map<ComponentTypeID, Vector<uint8>> ECS::m_Components;
Vector<IEntity*> ECS::m_Entities;

ECS::~ECS()
{
	/*for (Map<uint32, Array<uint8>>::iterator it = components.begin(); it != components.end(); ++it) {
		size_t typeSize = BaseECSComponent::getTypeSize(it->first);
		ECSComponentFreeFunction freefn = BaseECSComponent::getTypeFreeFunction(it->first);
		for (uint32 i = 0; i < it->second.size(); i += typeSize) {
			freefn((BaseECSComponent*)&it->second[i]);
		}
	}

	for (uint32 i = 0; i < entities.size(); i++) {
		delete entities[i];
	}*/
}

EntityHandle ECS::CreateEntity(BaseComponent** components, const ComponentTypeID* componentIDs, usize numComponents)
{
	IEntity* entity = new IEntity();
	entity->m_Id = (EntityID) m_Entities.Size();

	for (uint32 i = 0; i < numComponents; i++) {
		if (!BaseComponent::IsValidTypeID(componentIDs[i])) {
			delete entity;
			return NULL;
		}

		ECS::AddComponentInternal(*entity, componentIDs[i], components[i]);
	}

	m_Entities.EmplaceBack(entity);
	return entity;
}

void ECS::DeleteEntity(EntityHandle handle)
{
	IEntity* entity = (IEntity*)(handle);
	for (uint32 i = 0; i < entity->m_Components.Size(); i++) {
		DeleteComponent(entity->m_Components[i].first, entity->m_Components[i].second);
	}

	uint32 destIndex = entity->m_Id;
	uint32 srcIndex = (uint32) m_Entities.Size() - 1;
	delete m_Entities[destIndex];
	m_Entities[destIndex] = m_Entities[srcIndex];
	m_Entities[destIndex]->m_Id = destIndex;
	m_Entities.PopBack();
}

BaseComponent* ECS::AddComponentInternal(IEntity& entity, uint32 component_id, BaseComponent* component)
{
	ComponentCreateFunction createfn = BaseComponent::GetTypeCreateFunction(component_id);
	Vector<uint8>& components_buffer = m_Components[component_id];
	uint32 index_memory = createfn(components_buffer, &entity, component);
	entity.AddComponent(component_id, index_memory);
	return (BaseComponent*) components_buffer[index_memory];
}

void ECS::DeleteComponent(ComponentID id, uint32 mem_index)
{
	Vector<uint8>& components_buffer = m_Components[id];
	ComponentDeleteFunction freefn = BaseComponent::GetTypeDeleteFunction(id);
	uint32 size = BaseComponent::GetTypeSize(id);
	uint32 srcIndex = (uint32) components_buffer.Size() - size;

	BaseComponent* destComponent = (BaseComponent*) &components_buffer[mem_index];
	BaseComponent* srcComponent = (BaseComponent*) &components_buffer[srcIndex];
	freefn(destComponent);

	if (srcIndex == mem_index) {
		components_buffer.Resize(srcIndex);
		return;
	}

	std::memcpy(destComponent, srcComponent, size);
	IEntity* entity = (IEntity*) srcComponent->m_Entity;

	for (Pair<ComponentTypeID, uint32>& comp : entity->m_Components) {
		if (comp.first == id && comp.second == srcIndex) {
			comp.second = mem_index;
			break;
		}
	}

	components_buffer.Resize(srcIndex);
}

bool ECS::RemoveComponentInternal(IEntity& entity, uint32 component_id)
{
	auto& entity_components = entity.m_Components;
	usize comp_size = entity_components.Size();

	for (uint32 i = 0; i < comp_size; i++) {
		Pair<ComponentTypeID, uint32>& comp = entity_components[i];

		if (comp.first == component_id) {
			ECS::DeleteComponent(comp.first, comp.second);
			uint32 srcIndex = (uint32) entity.m_Components.Size() - 1;
			uint32 destIndex = i;
			entity.m_Components[destIndex] = entity.m_Components[srcIndex]; // Swap with the last component
			entity.m_Components.PopBack();
			return true;
		}
	}

	return false;
}

BaseComponent* ECS::GetComponentInternal(IEntity& entity, Vector<uint8>& component_buffer, uint32 component_id)
{
	for (Pair<ComponentTypeID, uint32>& comp : entity.m_Components) {
		if (comp.first == component_id) {
			return (BaseComponent*) &component_buffer[comp.second];
		}
	}

	return NULL;
}

void ECS::UpdateSystems(SystemList& system_list, float delta)
{
	usize systems_sz = system_list.GetSize();
	Vector<BaseComponent*> component_param;
	Vector<Vector<uint8>*> component_arrays;

	for (uint32 i = 0; i < systems_sz; i++) {
		const Vector<uint32>& component_types = system_list[i]->GetComponentTypes();

		if (component_types.Size() == 1) {
			uint32 typeSize = BaseComponent::GetTypeSize(component_types[0]);
			Vector<uint8>& comp_buffer = m_Components[component_types[0]];

			for (uint32 j = 0; j < (uint32) comp_buffer.Size(); j += typeSize) {
				BaseComponent* component = (BaseComponent*) &comp_buffer[j];
				system_list[j]->UpdateComponents(delta, &component);
			}
		}else {
			UpdateSystemWithMultipleComponents(system_list, i, delta, component_types, component_param, component_arrays);
		}
	}
}

void ECS::UpdateSystemWithMultipleComponents(SystemList& system_list, uint32 index, float delta, const Vector<uint32>& component_types, Vector<BaseComponent*>& component_param, Vector<Vector<uint8>*>& component_arrays)
{
	const Vector<uint32>& component_flag = system_list[index]->GetComponentFlags();
	component_param.Resize(MAX(component_param.Size(), component_types.Size()));
	component_arrays.Resize(MAX(component_arrays.Size(), component_types.Size()));

	for (uint32 i = 0; i < component_types.Size(); i++) {
		component_arrays[i] = &m_Components[component_types[i]];
	}

	uint32 min_size_index = ECS::FindLeastCommonComponent(component_types, component_flag);
	uint32 typeSize = BaseComponent::GetTypeSize(component_types[min_size_index]);
	Vector<uint8>& min_comp_buffer = *component_arrays[min_size_index];
	
	for (uint32 i = 0; i < (uint32) min_comp_buffer.Size(); i += typeSize) {
		component_param[min_size_index] = (BaseComponent*) &min_comp_buffer[i];
		IEntity* entity = static_cast<IEntity*>(component_param[min_size_index]->m_Entity);

		bool is_valid = true;
		for (uint32 j = 0; j < component_types.Size(); j++) {
			if (j == min_size_index) {
				continue;
			}

			component_param[j] = ECS::GetComponentInternal(*entity, *component_arrays[j], component_types[j]);
			if (component_param[j] == NULL && (component_flag[j] & BaseSystem::FLAG_OPTIONAL) == 0) {
				is_valid = false;
				break;
			}
		}

		if (is_valid) {
			system_list[index]->UpdateComponents(delta, &component_param[0]);
		}
	}
}

uint32 ECS::FindLeastCommonComponent(const Vector<uint32>& component_types, const Vector<uint32>& component_flags)
{
	uint32 min_index = (uint32) -1;
	uint32 min_size = (uint32) -1;

	for (uint32 i = 0; i < component_types.Size(); i++) {
		if ((component_flags[i] & BaseSystem::FLAG_OPTIONAL) != 0)  {
			continue;
		}

		uint32 type_size = BaseComponent::GetTypeSize(component_types[i]);
		uint32 size = (uint32) m_Components[component_types[i]].Size() / type_size;

		if (size <= min_size) {
			min_size = size;
			min_index = i;
		}
	}

	return min_index;
}

TRE_NS_END