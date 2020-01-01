#include "EntityManager.hpp"
#include <Core/ECS/World/World.hpp>
#include <Core/ECS/Component/BaseComponent.hpp>

TRE_NS_START

EntityManager::EntityManager(World* world) : m_World(world)
{

}

World& EntityManager::GetWorld()
{ 
	return *m_World; 
}

Archetype& EntityManager::CreateArchetype(const Bitset& signature)
{
	ArchetypeContainer::Object& archetype_pair = m_Archetypes.Put(this, signature);
	Archetype& archetype = archetype_pair.second;
	archetype.SetID(archetype_pair.first);
	m_SigToArchetypes.Emplace(signature, archetype_pair.first);
	return archetype;
}

Archetype& EntityManager::GetOrCreateArchetype(const Bitset& sig)
{
	uint32* arche_index;

	if ((arche_index = m_SigToArchetypes.GetKeyPtr(sig)) == NULL) {
		return EntityManager::CreateArchetype(sig);
	}

	return m_Archetypes[*arche_index];
}

Archetype* EntityManager::GetArchetype(const Bitset& sig)
{
	uint32* index;
	if ((index = m_SigToArchetypes.GetKeyPtr(sig)) != NULL) {
		return &m_Archetypes[*index];
	}

	return NULL;
}

Entity& EntityManager::CreateEntity(BaseComponent** components, const ComponentTypeID* componentIDs, usize numComponents)
{
	Entity& entity = this->CreateEntity();
	Bitset sig(BaseComponent::GetComponentsCount());

	for (uint32 i = 0; i < numComponents; i++) {
		sig.Set(componentIDs[i], true);
	}

	GetOrCreateArchetype(sig).AddEntityComponents(entity, components, componentIDs, numComponents);
	return entity;
}

void EntityManager::DeleteEntity(EntityHandle handle)
{
	Entity* entity = (Entity*)(handle);
	ArchetypeChunk* chunk = entity->GetChunk();
	chunk->DestroyEntityComponents(*entity);

	uint32 destIndex = entity->m_Id;
	ssize srcIndex = (ssize)m_Entities.Size() - 1;

	if (srcIndex > 0) {
		m_Entities[destIndex] = m_Entities[srcIndex];
		m_Entities[destIndex].m_Id = destIndex;
		m_Entities[destIndex].GetChunk()->GetEntityID(m_Entities[destIndex].m_InternalId) = destIndex;
	}

	m_Entities.PopBack();
}

BaseComponent* EntityManager::AddComponentInternal(Entity& entity, uint32 component_id, BaseComponent* component)
{
	ArchetypeChunk* chunk = entity.GetChunk();

	if (chunk) { // the entity already had some components
		Archetype& old_arche = chunk->GetArchetype();

		if (old_arche.GetSignature().Get(component_id))
			return EntityManager::GetComponentInternal(entity, component_id);

		Bitset sig = old_arche.GetSignature();
		sig.Set(component_id, true);
		uint32 old_internal_id = entity.m_InternalId;
		Archetype& archetype = EntityManager::GetOrCreateArchetype(sig);
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
	Archetype& archetype = EntityManager::GetOrCreateArchetype(sig);
	ArchetypeChunk* new_chunk = archetype.AddEntity(entity);
	return new_chunk->AddComponentToEntity(entity, component, component_id);
}

bool EntityManager::RemoveComponentInternal(Entity& entity, uint32 component_id)
{
	ArchetypeChunk* chunk = entity.GetChunk();

	if (chunk) { // the entity already had some components
		Archetype& old_arche = chunk->GetArchetype();

		if (!old_arche.GetSignature().Get(component_id))
			return false;

		if (old_arche.GetComponentsTypesCount() == 1) {
			chunk->RemoveEntityComponents(entity);
			entity.m_InternalId = 0;
			entity.m_Chunk = NULL;
			return true;
		}

		Bitset sig = old_arche.GetSignature();
		sig.Set(component_id, false);
		uint32 old_internal_id = entity.m_InternalId;
		Archetype& archetype = EntityManager::GetOrCreateArchetype(sig);
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

BaseComponent* EntityManager::GetComponentInternal(const Entity& entity, uint32 component_id)
{
	ArchetypeChunk* chunk = entity.GetChunk();
	ASSERTF(!(chunk && chunk->GetArchetype().GetSignature().Get(component_id)), "Invalid usage of GetComponentInternal entity doesn't have any components or doesn't have the specified component.");
	return chunk->GetComponent(entity, component_id);
}

Vector<BaseComponent*> EntityManager::GetAllComponents(ComponentTypeID id)
{
	Vector<BaseComponent*> res;
	Bitset sig(BaseComponent::GetComponentsCount());
	sig.Set(id, true);
	uint32 size = (uint32)BaseComponent::GetTypeSize(id);

	for (const ArchetypeContainer::Object& arche_pair : m_Archetypes) {
		const Archetype& arche = arche_pair.second;
		
		if ((arche.GetSignature() & sig) == sig) {

			for (const ArchetypeChunk& chunk : arche) {

				uint8* comp_buffer = chunk.GetComponentBuffer(id);

				for (uint32 i = 0; i < chunk.GetEntitiesCount(); i++) {
					res.EmplaceBack((BaseComponent*) &comp_buffer[i * size]);
				}
			}
		}
	}

	return res;
}

Map<ComponentTypeID, Vector<BaseComponent*>> EntityManager::GetAllComponentsMatchSignture(const Bitset& signature)
{
	Map<ComponentTypeID, Vector<BaseComponent*>> res;
	
	for (const ArchetypeContainer::Object& arche_pair : m_Archetypes) {
		const Archetype& arche = arche_pair.second;

		if ((arche.GetSignature() & signature)) {
			for (auto& c : arche.GetTypesBufferMarker()) {
				if (signature.Get(c.first)) {
					uint32 size = (uint32)BaseComponent::GetTypeSize(c.first);

					for (const ArchetypeChunk& chunk : arche) {
						uint8* comp_buffer = chunk.GetComponentBuffer(c.first);

						for (uint32 i = 0; i < chunk.GetEntitiesCount(); i++) {
							res[c.first].EmplaceBack((BaseComponent*) &comp_buffer[i * size]);
						}
					}
				}
			}
		}
	}

	return res;
}

TRE_NS_END