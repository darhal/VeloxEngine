#include "Entity.hpp"
#include <Core/ECS/Archetype/Archetype.hpp>
#include <Core/ECS/Entity/Entity.hpp>

TRE_NS_START

uint32 Entity::AttachToArchetype(const Archetype& arcehtype, EntityID internal_id)
{
	m_ArchetypeId = arcehtype.GetID();
	m_InternalId = internal_id;
	return m_InternalId;
}

uint32 Entity::AttachToArchetypeChunk(const ArchetypeChunk& archetype_chunk, EntityID internal_id)
{
	// m_ArchetypeId = arcehtype.GetID();
	m_InternalId = internal_id;
	return m_InternalId;
}

TRE_NS_END