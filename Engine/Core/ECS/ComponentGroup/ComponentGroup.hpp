#pragma once

#include <Core/Misc/Defines/Common.hpp>
#include <Core/DataStructure/Bitset/Bitset.hpp>
#include <Core/DataStructure/Vector/Vector.hpp>

TRE_NS_START

class Archetype;

class ComponentGroup
{
public:
	ComponentGroup(const Bitset& sig);


private:
	Vector<Archetype*> m_Archetypes;
	Bitset m_Signature;
};

TRE_NS_END