#pragma once

#include <Core/Misc/Defines/Common.hpp>
#include <Core/ECS/CommandFunctions/CommandFunctions.hpp>
#include <Core/ECS/Entity/Entity.hpp>

TRE_NS_START

struct ECSCommands 
{
	typedef void(*CommandFunction)(const void*);

	struct RemoveComponentCmd
	{
		World* word;
		EntityID entity_id;
		ComponentTypeID component_type_id;

		CONSTEXPR static CommandFunction DISPATCH_FUNCTION = &CommandFunctions::RemoveComponent;
	};
};

TRE_NS_END