#pragma once

#include <Core/Misc/Defines/Common.hpp>
#include <Core/DataStructure/Vector/Vector.hpp>
#include <Core/ECS/Common.hpp>

TRE_NS_START

struct ComponentGroup
{
	Vector<ComponentTypeID> All;
	Vector<ComponentTypeID> Any;
	Vector<ComponentTypeID> None;
};

TRE_NS_END
