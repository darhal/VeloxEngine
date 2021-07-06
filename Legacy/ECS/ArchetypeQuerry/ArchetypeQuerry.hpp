#pragma once

#include <Legacy/Misc/Defines/Common.hpp>
#include <Legacy/DataStructure/Vector.hpp>
#include <Legacy/ECS/Common.hpp>
#include <Legacy/DataStructure/Bitset.hpp>

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
