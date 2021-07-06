#pragma once

#include <Legacy/Misc/Defines/Common.hpp>
#include <Legacy/DataStructure/Bitset.hpp>
#include <Legacy/DataStructure/Vector.hpp>
#include <Legacy/ECS/ArchetypeQuerry/ArchetypeQuerry.hpp>

TRE_NS_START

class Archetype;

class ComponentGroup
{
public:
	ComponentGroup() {};

	ComponentGroup(const ArchetypeQuerry& querry);

	void QuerryArchetypes(EntityManager& ent_manager);

	void AddArchetypeIfMatch(Archetype& arche);

	const Vector<Archetype*> GetArchetypes() const { return m_Archetypes; }
private:
	Vector<Archetype*> m_Archetypes;
	ArchetypeQuerry m_ArchQuerry;
};

TRE_NS_END
