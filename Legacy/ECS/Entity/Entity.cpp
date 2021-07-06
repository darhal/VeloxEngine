#include "Entity.hpp"
#include <Legacy/ECS/Archetype/Archetype.hpp>
#include <Legacy/ECS/Entity/Entity.hpp>

TRE_NS_START

uint32 Entity::AttachToArchetypeChunk(ArchetypeChunk* archetype_chunk, EntityID internal_id)
{
	m_Chunk = archetype_chunk;
	m_InternalId = internal_id;
	return m_InternalId;
}

TRE_NS_END