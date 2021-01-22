#pragma once

#include "ResourcesManager.hpp"
#include <Renderer/Backend/Commands/Commands.hpp>

TRE_NS_START

namespace RenderResources
{
	template<typename T, typename... Args>
	Commands::CreateResourceCmd<T>* Create(uint32& res_id, Args&&... args)
	{
		ResourcesManager& manager = ResourcesManager::Instance();
		res_id = manager.CreateResource<T>(std::forward<Args>(args)...);
		Commands::CreateResourceCmd<T>* cmd = manager.GetContextOperationsQueue().SubmitCommand<Commands::CreateResourceCmd<T>>();
		cmd->resource = &manager.Get<T>(res_id);;
		return cmd;
	}
}

TRE_NS_END