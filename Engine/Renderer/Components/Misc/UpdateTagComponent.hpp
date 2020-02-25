#pragma once

#include <Core/Misc/Defines/Common.hpp>
#include <Core/ECS/Component/BaseComponent.hpp>

TRE_NS_START

struct UpdateTag : public Component<UpdateTag>
{
	// Empty tag component
};

TRE_NS_END