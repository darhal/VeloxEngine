#include "EntityContainer.hpp"
#include <Legacy/ECS/Entity/Entity.hpp>
#include <Legacy/Memory/Utils/Utils.hpp>

TRE_NS_START

EntityContainer::EntityContainer(EntityManager* manager, uint32 capacity) :
	m_Manager(manager), m_Entities(NULL), m_Capacity(capacity), m_ObjectCount(0),
	m_FreelistEnqueue(m_Capacity - 1), m_FreelistDequeue(0)
{
	void* data = Allocate<uint8>((sizeof(Entity) + sizeof(Index)) * m_Capacity);
	m_Entities = (Entity*) data;
	Index* indices = this->GetIndexArray();

	for (uint32 i = 0; i < m_Capacity; i++) {
		indices[i].index = INVALID_INDEX;
		indices[i].next_free = i + 1;
	}
}

Entity& EntityContainer::CreateEntity()
{
	if (m_ObjectCount >= m_Capacity) {
		this->Reallocate(m_Capacity * 2);
	}

	ID id = m_FreelistDequeue;
	Index& in = this->GetIndexArray()[id];
	m_FreelistDequeue = in.next_free;
	in.index = m_ObjectCount++;
	Entity* entity = new (&m_Entities[in.index]) Entity(m_Manager, id);
	return *entity;
}

void EntityContainer::Remove(ID id)
{
	Index* indices = this->GetIndexArray();
	Index& in = indices[id]; // Get index
	Entity& entity_to_delete = m_Entities[in.index]; // Get Object to delete
	ID old_id = entity_to_delete.GetEntityID();

	Entity& last_element = m_Entities[--m_ObjectCount]; // Get last element
	new (&entity_to_delete) Entity(::std::move(last_element)); // Swap last element and element to delete
	
	/*ArchetypeChunk* chunk = entity_to_delete.GetChunk(); // No need for this because m_Id wont change anyways
	if (chunk)
		chunk->GetEntityID(entity_to_delete.m_InternalId) = entity_to_delete.m_Id; // Update its ID in the chunk*/

	indices[entity_to_delete.GetEntityID()].index = in.index; // object to delete here is actually the last element we just swapped
	in.index = INVALID_INDEX; // Invalidate old index
	indices[m_FreelistEnqueue].next_free = id;
	m_FreelistEnqueue = id;
}

Entity* EntityContainer::Lookup(ID id) const
{
	Index* indices = this->GetIndexArray();
	Index& in = indices[id];

	if (in.index == INVALID_INDEX)
		return NULL;

	return &m_Entities[in.index];
}

void EntityContainer::Reallocate(uint32 new_capacity)
{
	Entity* ent_mem = (Entity*)Allocate<uint8>((sizeof(Entity) + sizeof(Index)) * new_capacity);
	Index* ind_mem = (Index*)((uint8*)ent_mem + sizeof(Entity) * new_capacity);

	memcpy(ent_mem, m_Entities, m_ObjectCount * sizeof(Entity));
	memcpy(ind_mem, this->GetIndexArray(), m_Capacity * sizeof(Index));

	uint32 start = new_capacity - m_Capacity;
	m_Capacity = new_capacity;
	m_Entities = (Entity*)ent_mem;

	for (uint32 i = start; i < m_Capacity; i++) {
		ind_mem[i].index = INVALID_INDEX;
		ind_mem[i].next_free = i + 1;
	}
}

Entity& EntityContainer::Get(ID id) const
{
	Index* indices = this->GetIndexArray();
	Index& in = indices[id];

	ASSERTF(in.index == INVALID_INDEX, "Invalid ID supplied to the PackedArray:Get().\n");
	return m_Entities[in.index];
}

EntityContainer::Index* EntityContainer::GetIndexArray() const 
{ 
	return (Index*)((uint8*)m_Entities + sizeof(Entity) * m_Capacity); 
}


TRE_NS_END