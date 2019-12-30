#include "ECS.hpp"
#include <Core/DataStructure/Utils/Utils.hpp>

TRE_NS_START

Vector<Entity> ECS::m_Entities;
PackedArray<Archetype> ECS::m_Archetypes;
HashMap<Bitset, uint32> ECS::m_SigToArchetypes;

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


Archetype& ECS::GetOrCreateArchetype(const Bitset& sig)
{
	uint32* arche_index;

	if ((arche_index = m_SigToArchetypes.GetKeyPtr(sig)) == NULL) {
		return ECS::CreateArchetype(sig);
	}

	return m_Archetypes[*arche_index];
}

Entity& ECS::CreateEntity(BaseComponent** components, const ComponentTypeID* componentIDs, usize numComponents)
{
	Entity& entity = m_Entities.EmplaceBack((EntityID)m_Entities.Size());

	Bitset sig(BaseComponent::GetComponentsCount());
	for (uint32 i = 0; i < numComponents; i++) {
		sig.Set(i, true);
	}

	GetOrCreateArchetype(sig).AddEntityComponents(entity, components, componentIDs, numComponents);
	return entity;
}

void ECS::DeleteEntity(EntityHandle handle)
{
	Entity* entity = (Entity*)(handle);
	ArchetypeChunk* chunk = entity->GetChunk();
	chunk->DestroyEntityComponents(*entity);

	uint32 destIndex = entity->m_Id;
	ssize srcIndex = (ssize) m_Entities.Size() - 1;

	if (srcIndex > 0) {
		m_Entities[destIndex] = m_Entities[srcIndex];
		m_Entities[destIndex].m_Id = destIndex;
	}
	
	m_Entities.PopBack();
}

BaseComponent* ECS::AddComponentInternal(Entity& entity, uint32 component_id, BaseComponent* component)
{
	ArchetypeChunk* chunk = entity.GetChunk();

	if (chunk) { // the entity already had some components
		Archetype& old_arche = chunk->GetArchetype();
		Bitset sig = old_arche.GetSignature();
		sig.Set(component_id, true);
		uint32 old_internal_id = entity.m_InternalId;
		Archetype& archetype = ECS::GetOrCreateArchetype(sig);
		ArchetypeChunk* new_chunk = archetype.AddEntity(entity);

		for (auto& c : old_arche.GetTypesBufferMarker()) {
			BaseComponent* old_comp = chunk->GetComponent(old_internal_id, chunk->GetComponentsBuffer() + c.second, c.first);
			new_chunk->AddComponentToEntity(entity, old_comp, c.first);
		}

		chunk->RemoveEntityComponents(old_internal_id);
		return new_chunk->AddComponentToEntity(entity, component, component_id);
	}  

	// entity without components
	Bitset sig(BaseComponent::GetComponentsCount());
	sig.Set(component_id, true);
	Archetype& archetype = ECS::GetOrCreateArchetype(sig);
	ArchetypeChunk* new_chunk = archetype.AddEntity(entity);
	return new_chunk->AddComponentToEntity(entity, component, component_id);
}

bool ECS::RemoveComponentInternal(Entity& entity, uint32 component_id)
{
	ArchetypeChunk* chunk = entity.GetChunk();

	if (chunk) { // the entity already had some components
		Archetype& old_arche = chunk->GetArchetype();

		if (old_arche.GetComponentsTypesCount() == 1) {
			chunk->RemoveEntityComponents(entity);
			entity.m_InternalId = 0;
			entity.m_Chunk = NULL;
			return true;
		}

		Bitset sig = old_arche.GetSignature();
		sig.Set(component_id, false);
		uint32 old_internal_id = entity.m_InternalId;
		Archetype& archetype = ECS::GetOrCreateArchetype(sig);
		ArchetypeChunk* new_chunk = archetype.AddEntity(entity);

		for (auto& c : old_arche.GetTypesBufferMarker()) {
			if (c.first == component_id)
				continue;

			BaseComponent* old_comp = chunk->GetComponent(old_internal_id, chunk->GetComponentsBuffer() + c.second, c.first);
			new_chunk->AddComponentToEntity(entity, old_comp, c.first);
		}

		chunk->RemoveEntityComponents(old_internal_id);
		return true;
	}  

	// entity without components
	return false;
}

BaseComponent* ECS::GetComponentInternal(const Entity& entity, uint32 component_id)
{
	return entity.GetChunk()->GetComponent(entity, component_id);
}

void ECS::UpdateSystems(SystemList& system_list, float delta)
{
	usize systems_sz = system_list.GetSize();

	for (uint32 i = 0; i < systems_sz; i++) {
		BaseSystem& system = *system_list[i];
		const Bitset& sig = system.GetSignature();
		uint32* index;

		if ((index = m_SigToArchetypes.GetKeyPtr(sig)) != NULL) {
			Archetype& arche = m_Archetypes[*index];

			if (!arche.IsEmpty()) {
				for (auto& c : arche.GetTypesBufferMarker()) {
					ArchetypeChunk* chunk = arche.GetLastOccupiedChunk();
					uint8* comp_buffer = chunk->GetComponentsBuffer() + c.second; // Get the coomponent buffer
					uint32 size = (uint32) BaseComponent::GetTypeSize(c.first);
					// printf("[ECS] SIG = %s - ComponentID = %d - Comp buffer : %p\n", Utils::ToString(sig), c.first, comp_buffer);

					do {
						for (uint32 i = 0; i < chunk->GetEntitiesCount(); i++) {
							system.UpdateComponents(delta, c.first, (BaseComponent*) &comp_buffer[i * size]);
						}

						chunk = chunk->GetNextChunk();
					} while (chunk);
				}
			}
		}
	}
}


/*BaseComponent* ECS::AddComponentInternal(Entity& entity, uint32 component_id, BaseComponent* component)
{
	if (entity.m_Chunk) {
		EntityID old_internal_id = entity.m_InternalId;
		ArchetypeChunk& chunk = *entity.m_Chunk;
		Archetype& old_archetype = chunk.GetArchetype();
		const Bitset& old_sig = old_archetype.GetSignature();
		Bitset new_sig{ old_sig };
		new_sig.Set(component_id, true);
		uint32* arche_index;
		//printf("[ADD COMPONENT] New signature : %s - Old Singature : %s\n", Utils::ToString(new_sig).Buffer(), Utils::ToString(old_sig).Buffer());
		
		if ((arche_index = m_SigToArchetypes.GetKeyPtr(new_sig)) == NULL) {
			// Creating new archetype
			Vector<ComponentTypeID> types { old_archetype.GetComponentTypes() };
			types.EmplaceBack(component_id);
			Archetype& archetype = ECS::CreateArchetype(new_sig, types);
			ArchetypeChunk* new_chunk = archetype.GetAllocationChunk();
			new_chunk->AddEntity(entity);

			// Get old components from the former achetype
			for (auto& id : chunk.GetBufferMap()) {
				// Add old components to the new archetype
				new_chunk->AddComponentToEntity(entity, chunk.GetComponent(old_internal_id, id.second, id.first), id.first);
			}

			// Remove components from the old archetype
			if (!chunk.RemoveEntityComponents(entity)) { // careful here destroctur must no be called
				//m_SigToArchetypes.Remove(old_sig); // keep this order its on purpose
				// m_Archetypes.Remove(entity.m_ArchetypeId); // Remove it from the table
			}

			// add the newly added component
			return new_chunk->AddComponentToEntity(entity, component, component_id);
		} else { // There is already existing achetype that match the signature
			//printf("[ADD COMPONENT] There is already existing achetype that match the signature.\n");
			Archetype& archetype = m_Archetypes[*arche_index];
			ArchetypeChunk* new_chunk = archetype.GetAllocationChunk();
			new_chunk->AddEntity(entity);

			// Get old components from the former achetype
			for (auto& id : chunk.GetBufferMap()) {
				// Add old components to the new archetype
				new_chunk->AddComponentToEntity(entity, chunk.GetComponent(old_internal_id, id.second, id.first), id.first);
			}

			// Remove components from the old archetype
			if (!chunk.RemoveEntityComponents(entity)) { // careful here destroctur must no be called
				//m_SigToArchetypes.Remove(old_sig); // keep this order its on purpose
				// m_Archetypes.Remove(entity.m_ArchetypeId); // Remove it from the table
			}

			// add the newly added component
			return new_chunk->AddComponentToEntity(entity, component, component_id);
		}
	} else {
		Bitset new_sig(BaseComponent::GetComponentsCount());
		new_sig.Set(component_id, true);
		uint32* arche_index;

		if ((arche_index = m_SigToArchetypes.GetKeyPtr(new_sig)) == NULL) {
			//printf("[ADD COMPONENT] The entity doesnt have archetype we will create one (Signature : %s).\n", Utils::ToString(new_sig).Buffer());
			// Creating new archetype
			Archetype& archetype = ECS::CreateArchetype(new_sig);
			entity.AttachToArchetype(archetype, archetype.m_EntitiesCount++); // Attach the entity to the archetype and entity_count.

			// add the newly added component
			archetype.AddComponentType(component_id);
			return archetype.AddComponentToEntity(entity, component, component_id);
		} else {
			//printf("[ADD COMPONENT] Entity doesnt have previous archetype, There is already existing achetype that match the signature.\n");
			Archetype& archetype = m_Archetypes[*arche_index];
			archetype.AddEntity(entity);
			// add the newly added component
			return archetype.UpdateComponentMemory(entity, component, component_id);
		}
	}

	return NULL;
}

bool ECS::RemoveComponentInternal(Entity& entity, uint32 component_id)
{
	if (entity.m_ArchetypeId == EntityID(-1)) return false;

	Archetype& old_archetype = m_Archetypes[entity.m_ArchetypeId];
	const Bitset& old_sig = old_archetype.GetSignature();
	Bitset null_bitset(BaseComponent::GetComponentsCount());
	Bitset new_sig{ old_archetype.GetSignature() };
	new_sig.Set(component_id, false);
	bool is_null = new_sig == null_bitset;
	uint32* arche_index;

	if (is_null) {
		if (!old_archetype.DestroyEntityComponents(entity)) { // Call dtor
			// printf("Old archetype become empty: Signature : %s | ID: %d\n", Utils::ToString(old_sig).Buffer(), entity.m_ArchetypeId);
			m_SigToArchetypes.Remove(old_sig); // keep this order its on purpose
			m_Archetypes.Remove(entity.m_ArchetypeId); // Remove it from the table
		}

		entity.m_ArchetypeId = -1;
		entity.m_InternalId = -1;
		//printf("[REMOVE COMPONENT] Signature become null .\n");
		return true;
	}
	// printf("[REMOVE COMPONENT] New signature : %s - Old Singature : %s\n", Utils::ToString(new_sig).Buffer(), Utils::ToString(old_sig).Buffer());
	if ((arche_index = m_SigToArchetypes.GetKeyPtr(new_sig)) == NULL) {
		if (old_archetype.GetEntitesCount() == 1) {
			//printf("[REMOVE COMPONENT] The old archetype have only one entity and can be transformed.\n");
			// Update the signature to archetype
			m_SigToArchetypes.Remove(old_sig);
			m_SigToArchetypes.Emplace(new_sig, entity.m_ArchetypeId);
			// The old achetype can be transformed to new archetype, these are done in this order on purpose
			// So that the components dtor get called before the memory buffer is wiped!
			old_archetype.DestroyComponentHelper(old_archetype.GetComponentBuffer(component_id), component_id, entity.m_InternalId); // Call dtor
			old_archetype.RemoveComponentType(component_id);
			return true;
		} else { 
			// Creating new archetype
			//printf("[REMOVE COMPONENT] Creating new archetype.\n");
			Archetype& archetype = ECS::CreateArchetype(new_sig);
			EntityID old_internal_id = entity.m_InternalId;

			// Get old components from the former achetype
			for (auto& components : old_archetype.GetComponentsBuffer()) {
				if (components.first == component_id) { //  Don't add the component we are deleting
					// Call component dtor we are removing, but dont swap
					old_archetype.DestroyComponentHelper(components.second, components.first, entity.m_InternalId);
					continue;
				}

				// Add other old components to the new archetype
				archetype.AddComponentType(components.first);
				archetype.AddComponentToEntity(entity, old_archetype.GetComponentInternal(entity, components.second, components.first), components.first);
			}

			// Remove components from the old archetype and check wether the old archetype is empty
			if (!old_archetype.RemoveEntityComponents(entity)) { // Component dtor must not be called here
				// printf("Old archetype become empty: Signature : %s | ID: %d\n", Utils::ToString(old_sig).Buffer(), entity.m_ArchetypeId);
				m_SigToArchetypes.Remove(old_sig); // keep this order its on purpose
				m_Archetypes.Remove(entity.m_ArchetypeId); // Remove it from the table
			}

			entity.AttachToArchetype(archetype, archetype.m_EntitiesCount++); // Attach the entity to the archetype and increment its entity_count.
		}
	} else {
		// There is already existing achetype that match the signature
		Archetype& archetype = m_Archetypes[*arche_index];
		EntityID inetnal_id = archetype.ReseveEntity(); // Reserve place for the new entity
		//printf("[REMOVE COMPONENT] There is already existing achetype that match the signature %s and its index is %d.\n", 
		//	Utils::ToString(new_sig).Buffer(), archetype.GetID());

		// Get old components from the former achetype
		for (auto& components : old_archetype.GetComponentsBuffer()) {
			if (components.first == component_id) { //  Don't add the component we are deleting
				// Call component dtor we are removing
				old_archetype.DestroyComponentHelper(components.second, components.first, entity.m_InternalId);
				continue;
			}

			// Add old components to the new archetype
			archetype.UpdateComponentMemoryInternal(inetnal_id, entity, old_archetype.GetComponentInternal(entity, components.second, components.first), components.first);
		}

		// Remove components from the old archetype and check wether the old archetype is empty
		if (!old_archetype.RemoveEntityComponents(entity)) { // Component dtor must not be called here
			// printf("Old archetype become empty: Signature : %s | ID: %d\n", Utils::ToString(old_sig).Buffer(), entity.m_ArchetypeId);
			m_SigToArchetypes.Remove(old_sig); // keep this order its on purpose
			m_Archetypes.Remove(entity.m_ArchetypeId); // Remove it from the table
		}

		entity.AttachToArchetype(archetype, inetnal_id);// attach the entity to the reserved place
	}

	return true;
}

BaseComponent* ECS::GetComponentInternal(const Entity& entity, uint32 component_id)
{
	return entity.GetChunk()->GetComponent(entity, component_id);
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
}*/


TRE_NS_END