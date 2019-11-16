#include "ECS.hpp"
#include <Core/DataStructure/Utils/Utils.hpp>

TRE_NS_START

Map<ComponentTypeID, Vector<uint8>> ECS::m_Components;
Vector<IEntity*> ECS::m_Entities;
Vector<Archetype> ECS::m_Archetypes;
HashMap<Bitset, uint32> ECS::m_SigToArchetypes;


void ECS::ShowDic(const char* label)
{
	printf("------------%s-------------\n", label);
	for (auto& p : ECS::m_SigToArchetypes) {
		printf("[KEY: %s VALUE: %d](Key ptr : %p)\n", Utils::ToString(p.first).Buffer(), p.second, m_SigToArchetypes.Get(p.first));
	}
}

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

	Bitset sig(BaseComponent::GetComponentsCount());
	for (uint32 i = 0; i < numComponents; i++) {
		sig.Set(i, true);
	}

	uint32* arche_index;
	if ((arche_index = m_SigToArchetypes.GetKeyPtr(sig)) == NULL) {
		EntityID archtypeid = (EntityID) m_Archetypes.Size();
		m_SigToArchetypes.Emplace(sig, archtypeid);

		Archetype& archetype = m_Archetypes.EmplaceBack(archtypeid);
		archetype.AddEntityComponents(*entity, components, componentIDs, numComponents);
	} else {
		m_Archetypes[entity->m_ArchetypeId].AddEntityComponents(*entity, components, componentIDs, numComponents);
	}

	m_Entities.EmplaceBack(entity);
	return entity;
}

void ECS::DeleteEntity(EntityHandle handle)
{
	IEntity* entity = (IEntity*)(handle);
	m_Archetypes[entity->m_ArchetypeId].RemoveEntityComponents(*entity);

	uint32 destIndex = entity->m_Id;
	ssize srcIndex = (ssize) m_Entities.Size() - 1;
	delete m_Entities[destIndex];

	if (srcIndex > 0) {
		m_Entities[destIndex] = m_Entities[srcIndex];
		m_Entities[destIndex]->m_Id = destIndex;
	}
	
	m_Entities.PopBack();
}

BaseComponent* ECS::AddComponentInternal(IEntity& entity, uint32 component_id, BaseComponent* component)
{
	if (entity.m_ArchetypeId != EntityID(-1)) {
		Archetype& old_archetype = m_Archetypes[entity.m_ArchetypeId];
		const Bitset& old_sig = old_archetype.GetSignature();
		Bitset new_sig{ old_sig };
		new_sig.Set(component_id, true);
		uint32* arche_index;
		printf("*New signature : %s - Old Singature : %s\n", Utils::ToString(new_sig).Buffer(), Utils::ToString(old_sig).Buffer());
		
		if ((arche_index = m_SigToArchetypes.GetKeyPtr(new_sig)) == NULL) {
			printf("No archetypes found.\n");
			if (old_archetype.GetEntitesCount() == 1) {
				// The old achetype can be transformed to new archetype that will contain our new signature
				printf("old archetypes can be transformed.\n");
				
				m_SigToArchetypes.Remove(old_sig);
				m_SigToArchetypes.Emplace(new_sig, entity.m_ArchetypeId);
				// Update the signature to archetype
				old_archetype.AddComponentType(component_id);
				return old_archetype.AddComponentToEntity(entity, component, component_id);
			} else { // We have to create new archetype
				printf("Creating new archetype.\n");
				// Creating new archetype
				EntityID archtypeid = (EntityID)m_Archetypes.Size();;
				m_SigToArchetypes.Emplace(new_sig, archtypeid);
				Archetype& archetype = m_Archetypes.EmplaceBack(archtypeid);

				// Get old components from the former achetype
				for (auto& components : old_archetype.GetComponentsBuffer()) {
					// Add old components to the new archetype
					archetype.AddComponentType(components.first);
					archetype.AddComponentToEntity(entity, old_archetype.GetComponentInternal(entity, components.second, components.first), components.first);
				}

				// Remove components from the old archetype
				old_archetype.RemoveEntityComponents(entity); //TODO: carefull here destroctur is being called, mdofiy it to not call it
				entity.AttachToArchetype(archetype, archetype.m_EntitiesCount++); // Attach the entity to the archetype and entity_count.
				// add the newly added component
				archetype.AddComponentType(component_id);
				return archetype.AddComponentToEntity(entity, component, component_id);
			}
		} else { // There is already existing achetype that match the signature
			printf("There is already existing achetype that match the signature.\n");
			Archetype& archetype = m_Archetypes[*arche_index];
			EntityID inetnal_id = archetype.ReseveEntity(); // Reserve place for the new entity

			// Get old components from the former achetype
			for (auto& components : old_archetype.GetComponentsBuffer()) {
				// Add old components to the new archetype
				archetype.UpdateComponentMemoryInternal(inetnal_id, entity, old_archetype.GetComponentInternal(entity, components.second, components.first), components.first);
			}

			// Remove components from the old archetype and check wether the old archetype is empty
			if (!old_archetype.RemoveEntityComponents(entity)) { //TODO: careful here destroctur is being called, mdofiy it to not call it
				m_SigToArchetypes.Remove(old_sig); // TODO: Remove it from the table
			}

			entity.AttachToArchetype(archetype, inetnal_id);// attach the entity to the reserved place
			// add the newly added component
			return archetype.UpdateComponentMemoryInternal(inetnal_id, entity, component, component_id);
		}
	} else {
		Bitset new_sig(BaseComponent::GetComponentsCount());
		new_sig.Set(component_id, true);
		// printf("Does contain key : %d | value : %d | Bitset : %s\n", m_SigToArchetypes.ContainsKey(new_sig), m_SigToArchetypes.Get(new_sig), Utils::ToString(new_sig));
		uint32* arche_index;

		if ((arche_index = m_SigToArchetypes.GetKeyPtr(new_sig)) == NULL) {
			printf("The entity doesnt have archetype we will create one (Signature : %s).\n", Utils::ToString(new_sig).Buffer());
			// Creating new archetype
			EntityID archtypeid = (EntityID)m_Archetypes.Size();;
			m_SigToArchetypes.Emplace(new_sig, archtypeid);
			Archetype& archetype = m_Archetypes.EmplaceBack(archtypeid);
			entity.AttachToArchetype(archetype, archetype.m_EntitiesCount++); // Attach the entity to the archetype and entity_count.

			// add the newly added component
			archetype.AddComponentType(component_id);
			return archetype.AddComponentToEntity(entity, component, component_id);
		} else {
			printf("There is already existing achetype that match the signature.\n");
			Archetype& archetype = m_Archetypes[*arche_index];
			archetype.AddEntity(entity);
			// add the newly added component
			return archetype.UpdateComponentMemory(entity, component, component_id);
		}
	}

	return NULL;
}

bool ECS::RemoveComponentInternal(IEntity& entity, uint32 component_id)
{
	if (entity.m_ArchetypeId == EntityID(-1)) return false;

	Archetype& old_archetype = m_Archetypes[entity.m_ArchetypeId];
	const Bitset& old_sig = old_archetype.GetSignature();
	Bitset new_sig{ old_archetype.GetSignature() };
	new_sig.Set(component_id, false);
	uint32* arche_index;

	if ((arche_index = m_SigToArchetypes.GetKeyPtr(new_sig)) == NULL) {
		if (old_archetype.GetEntitesCount() == 1) {
			// Update the signature to archetype
			m_SigToArchetypes.Remove(old_sig);
			m_SigToArchetypes.Emplace(new_sig, entity.m_ArchetypeId);
			// The old achetype can be transformed to new archetype, these are done in this order on purpose
			// So that the components dtor get called before the memory buffer is wiped!
			old_archetype.RemoveComponent(old_archetype.GetComponentBuffer(component_id), component_id, entity.m_InternalId);
			old_archetype.RemoveComponentType(component_id);
			return true;
		} else { 
			// Creating new archetype
			EntityID archtypeid = (EntityID)m_Archetypes.Size();
			EntityID old_internal_id = entity.m_InternalId;
			m_SigToArchetypes.Emplace(new_sig, archtypeid);
			Archetype& archetype = m_Archetypes.EmplaceBack(archtypeid);

			// Get old components from the former achetype
			for (auto& components : old_archetype.GetComponentsBuffer()) {
				if (components.first == component_id) //  Don't add the component we are deleting
					continue;

				// Add other old components to the new archetype
				archetype.AddComponentType(components.first);
				archetype.AddComponentToEntity(entity, old_archetype.GetComponentInternal(entity, components.second, components.first), components.first);
			}

			// Remove components from the old archetype
			old_archetype.RemoveEntityComponents(entity); // Component dtor being called here
			entity.AttachToArchetype(archetype, archetype.m_EntitiesCount++); // Attach the entity to the archetype and increment its entity_count.
		}
	} else {
		// There is already existing achetype that match the signature
		Archetype& archetype = m_Archetypes[*arche_index];
		EntityID inetnal_id = archetype.ReseveEntity(); // Reserve place for the new entity

		// Get old components from the former achetype
		for (auto& components : old_archetype.GetComponentsBuffer()) {
			if (components.first == component_id) //  Don't add the component we are deleting
				continue;

			// Add old components to the new archetype
			archetype.UpdateComponentMemoryInternal(inetnal_id, entity, old_archetype.GetComponentInternal(entity, components.second, components.first), components.first);
		}

		// Remove components from the old archetype and check wether the old archetype is empty
		if (!old_archetype.RemoveEntityComponents(entity)) { //TODO: careful here destroctur is being called, mdofiy it to not call it
			m_SigToArchetypes.Remove(old_sig); // TODO: Remove it from the table
		}

		entity.AttachToArchetype(archetype, inetnal_id);// attach the entity to the reserved place
	}

	return true;
}

BaseComponent* ECS::GetComponentInternal(const Archetype& archetype, const IEntity& entity, uint32 component_id)
{
	return archetype.GetComponent(entity, component_id);
}

void ECS::UpdateSystems(SystemList& system_list, float delta)
{
	usize systems_sz = system_list.GetSize();

	for (uint32 i = 0; i < systems_sz; i++) {
		BaseSystem& system = *system_list[i];
		const Bitset& sig = system.GetSignature();

		// printf("[CHECKING] SIGNATURE : %s (Key ptr : %p)\n", Utils::ToString(sig).Buffer(), m_SigToArchetypes.GetKeyPtr(sig));
		uint32* index;

		if ((index = m_SigToArchetypes.GetKeyPtr(sig)) != NULL) {

			//printf("[UPDATE] Archetype index : %d | System Bitset : %s | Number of entites to update : %d\n", *index,
			//	Utils::ToString(sig).Buffer(), m_Archetypes[*index].GetEntitesCount());

			for (auto& components : m_Archetypes[*index].GetComponentsBuffer()) {
				for (uint32 i = 0; i < components.second.Size(); i += BaseComponent::GetTypeSize(components.first)) {
					system.UpdateComponents(delta, components.first, (BaseComponent*)&components.second[i]);
				}
			}
		}
	}
}


TRE_NS_END