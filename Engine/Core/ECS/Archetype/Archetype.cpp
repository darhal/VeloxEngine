#include "Archetype.hpp"
#include "Chunk/ArchetypeChunk.hpp"
#include <Core/Memory/Allocators/LinearAlloc/LinearAllocator.hpp>

TRE_NS_START

Archetype::Archetype(const Bitset& bitset, const Vector<ComponentTypeID>& ids) :
	m_ComponentTypes(std::move(ids)),
	m_Signature(std::move(bitset)),
	m_FreeChunks(NULL), m_OccupiedChunks(NULL),
	m_ComponentsArraySize(0), m_Id(0)
{
	// Calculate size for all the components
	for (const ComponentTypeID& id : m_ComponentTypes) {
		m_ComponentsArraySize += BaseComponent::GetTypeSize(id) * ArchetypeChunk::CAPACITY;
	}
}

Archetype::Archetype(const Vector<ComponentTypeID>& ids) :
	m_ComponentTypes(std::move(ids)),
	m_Signature(BaseComponent::GetComponentsCount()),
	m_FreeChunks(NULL), m_OccupiedChunks(NULL),
	m_Id(0)
{
	// Calculate size for all the components
	for (const ComponentTypeID& id : m_ComponentTypes) {
		m_ComponentsArraySize += BaseComponent::GetTypeSize(id) * ArchetypeChunk::CAPACITY;
		m_Signature.Set(id, true);
	}
}

Archetype::Archetype(const Bitset& bitset) :
	m_ComponentTypes(),
	m_Signature(std::move(bitset)),
	m_FreeChunks(NULL), m_OccupiedChunks(NULL),
	m_Id(0)
{
	// Calculate size for all the components
	ComponentTypeID id = 0;

	for (uint32 i= 0; i < m_Signature.Length(); i++) {
		if (m_Signature.Get(i)) {
			m_ComponentsArraySize += BaseComponent::GetTypeSize(id) * ArchetypeChunk::CAPACITY;
			m_ComponentTypes.EmplaceBack(id);
		}

		id++;
	}
}

Archetype::~Archetype()
{
	ArchetypeChunk* next;

	while (m_FreeChunks) {
		next = m_FreeChunks;
		m_FreeChunks = m_FreeChunks->GetNextChunk();
		Free((uint8*)next);
	}

	while (m_OccupiedChunks) {
		next = m_OccupiedChunks;
		m_OccupiedChunks = m_OccupiedChunks->GetNextChunk();
		next->ArchetypeChunk::~ArchetypeChunk();
		Free((uint8*)next);
	}
}

void Archetype::AddComponentType(ComponentTypeID id)
{
	m_Signature.Set(id, true);
	m_ComponentTypes.EmplaceBack(id);
}

ArchetypeChunk* Archetype::GenerateChunk()
{
	usize cost_of_map_nodes = (m_ComponentTypes.Size() - 1) * sizeof(ArchetypeChunk::TypeBufferMap::MapTree::Node);
	usize total_chunk_size = sizeof(ArchetypeChunk) + cost_of_map_nodes + m_ComponentsArraySize;

	// Allocate
	uint8* total_buffer = Allocate<uint8>(total_chunk_size);
	ArchetypeChunk* archetypechunk;

	if (m_FreeChunks) {
		archetypechunk = new (total_buffer) ArchetypeChunk(this, LinearAllocator(cost_of_map_nodes, total_buffer + sizeof(ArchetypeChunk)));
	}else {
		archetypechunk = new (total_buffer) ArchetypeChunk(this, LinearAllocator(cost_of_map_nodes, total_buffer + sizeof(ArchetypeChunk)));
	}

	// Set it up
	uint8* offset = total_buffer + sizeof(ArchetypeChunk) + cost_of_map_nodes;
	for (const ComponentTypeID& id : m_ComponentTypes) {
		m_FreeChunks->AddComponentBuffer(id, offset);
		offset += BaseComponent::GetTypeSize(id) * ArchetypeChunk::CAPACITY;
	}

	return archetypechunk;
}

ArchetypeChunk* Archetype::GetAllocationChunk()
{
	if (m_OccupiedChunks && !m_OccupiedChunks->IsFull()) {
		return m_OccupiedChunks;
	} else {
		if (m_FreeChunks) {
			ArchetypeChunk* next_free = m_FreeChunks->GetNextChunk();
			m_FreeChunks->SetNextChunk(m_OccupiedChunks);
			m_OccupiedChunks = m_FreeChunks;
			m_FreeChunks = next_free;
			return m_OccupiedChunks;
		}else{
			m_OccupiedChunks = this->GenerateChunk();
			return m_OccupiedChunks;
		}
	}
}

ArchetypeChunk* Archetype::GetLastOccupiedChunk()
{
	return m_OccupiedChunks;
}

void Archetype::AddEntityComponents(Entity& entity, BaseComponent** components, const ComponentTypeID* componentIDs, usize numComponents)
{
	ArchetypeChunk* chunk = this->GetAllocationChunk();
	chunk->AddEntityComponents(entity, components, componentIDs, numComponents);
}

void Archetype::PushFreeChunk(ArchetypeChunk* chunk)
{
	m_OccupiedChunks = chunk->GetNextChunk();
	chunk->SetNextChunk(m_FreeChunks);
	m_FreeChunks = chunk;
}

TRE_NS_END