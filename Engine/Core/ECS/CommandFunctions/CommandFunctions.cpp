#include "CommandFunctions.hpp"
#include <Core/ECS/Commands/Commands.hpp>
#include <Core/ECS/World/World.hpp>

TRE_NS_START

void CommandFunctions::RemoveComponent(const void* data)
{
	const ECSCommands::RemoveComponentCmd* real_data = reinterpret_cast<const ECSCommands::RemoveComponentCmd*>(data);
		
	EntityManager& manager = real_data->word->GetEntityManager();
	Entity& ent = manager.GetEntityByID(real_data->entity_id);
	manager.RemoveComponentInternal(ent, real_data->component_type_id);
}

TRE_NS_END