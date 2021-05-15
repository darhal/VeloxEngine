#pragma once

#include <Core/Misc/Defines/Common.hpp>
#include <Core/DataStructure/Vector/Vector.hpp>
#include <Core/ECS/Common.hpp>
#include <Core/DataStructure/Bitset/Bitset.hpp>

TRE_NS_START

struct ArchetypeQuerry
{
	ArchetypeQuerry(const Bitset& all, const Bitset& any, const Bitset& none) :
		All(all), Any(any), None(none)
	{}

	ArchetypeQuerry() =  default;

	Bitset All;
	Bitset Any;
	Bitset None;
};


TRE_NS_END
