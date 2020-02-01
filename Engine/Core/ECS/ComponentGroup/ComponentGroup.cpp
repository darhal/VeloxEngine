#include "ComponentGroup.hpp"
#include <Core/ECS/EntityManager/EntityManager.hpp>

TRE_NS_START

ComponentGroup::ComponentGroup(const ArchetypeQuerry& querry) : 
	m_ArchQuerry(querry)
{
}

void ComponentGroup::QuerryArchetypes(EntityManager& ent_manager)
{
	m_Archetypes = ent_manager.GettAllArchetypeThatMatch(m_ArchQuerry);
}

void ComponentGroup::AddArchetypeIfMatch(Archetype& arche)
{
	const Bitset& arch_sig = arche.GetSignature();

	if (!(arch_sig & m_ArchQuerry.None) && ((arch_sig & m_ArchQuerry.Any) || ((arch_sig & m_ArchQuerry.All) == m_ArchQuerry.All))) {
		m_Archetypes.EmplaceBack(&arche);
	}
}

TRE_NS_END