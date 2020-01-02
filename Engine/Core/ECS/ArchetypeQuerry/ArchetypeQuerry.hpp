#pragma once

#include <Core/Misc/Defines/Common.hpp>
#include <Core/DataStructure/Vector/Vector.hpp>
#include <Core/ECS/Common.hpp>
#include <Core/DataStructure/Bitset/Bitset.hpp>

TRE_NS_START

struct ArchetypeQuerry
{
	Bitset All;
	Bitset Any;
	Bitset None;
};


TRE_NS_END
