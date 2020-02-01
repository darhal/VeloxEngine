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
	printf("[AddArchetypeIfMatch] [Signature:%s|None:%s|Any:%s|All:%s] -> %d|%d|%d\n", 
		Utils::ToString(arch_sig).Buffer(), Utils::ToString(m_ArchQuerry.None).Buffer(), 
		Utils::ToString(m_ArchQuerry.Any).Buffer(), Utils::ToString(m_ArchQuerry.All).Buffer(),

		!(arch_sig & m_ArchQuerry.None), !!(arch_sig & m_ArchQuerry.Any), 
		((arch_sig & m_ArchQuerry.All) == m_ArchQuerry.All)
	);
	// TODO: Fix this later the querry of All is not working!!!
	/*
	TagComponent1: 5 | TagComponent2: 4 | TestComponent: 2 | TestComponent2: 3
		[AddArchetypeIfMatch] [Signature:001110|None:000000|Any:000000|All:001100] -> 1|0|1
		[AddArchetypeIfMatch] [Signature:001100|None:000000|Any:000000|All:001100] -> 1|0|1
		[AddArchetypeIfMatch] [Signature:001101|None:000000|Any:000000|All:001100] -> 1|0|1
	*/

	if (!(arch_sig & m_ArchQuerry.None) && ((arch_sig & m_ArchQuerry.Any) || ((arch_sig & m_ArchQuerry.All) == m_ArchQuerry.All))) {
		
		m_Archetypes.EmplaceBack(&arche);
	}
}

TRE_NS_END