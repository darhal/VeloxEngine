#pragma once

#include <Core/Misc/Defines/Common.hpp>
#include "RenderEngine/Renderer/Backend/CommandBuffer/ResourcesCommandBucket/ResourcesCommandBucket.hpp"

TRE_NS_START

class RenderResourceContext
{
public:
	typedef RenderSettings::ResourcesCmdBuffer	 ResourcesCmdBuffer;

	const ResourcesCmdBuffer& GetResourcesCommandBuffer() const { return m_ResourcesCommandBuffer; }

	ResourcesCmdBuffer& GetResourcesCommandBuffer() { return m_ResourcesCommandBuffer; }

	template<typename Cmd>
	typename Cmd* CreateResource(typename Cmd::ID* out_id_ptr, const typename Cmd::Settings& settings, typename ResourcesCmdBuffer::Key key = 0);

	template<typename Cmd, typename Cmd2>
	typename Cmd* CreateResourceAfter(Cmd2* cmd, typename Cmd::ID* out_id_ptr, const typename Cmd::Settings& settings);

	FORCEINLINE void Update();

private:
	ResourcesCmdBuffer m_ResourcesCommandBuffer;
};

template<typename Cmd>
typename Cmd* RenderResourceContext::CreateResource(typename Cmd::ID* out_id_ptr, const typename Cmd::Settings& settings, typename ResourcesCmdBuffer::Key key)
{
	Cmd* res_creation_cmd = m_ResourcesCommandBuffer.AddCommand<Cmd>(key);
	res_creation_cmd->resource = ResourcesManager::GetGRM().Create<typename Cmd::Resource>(out_id_ptr);
	res_creation_cmd->settings = settings;
	return res_creation_cmd;
}

template<typename Cmd, typename Cmd2>
typename Cmd* RenderResourceContext::CreateResourceAfter(Cmd2* cmd, typename Cmd::ID* out_id_ptr, const typename Cmd::Settings& settings)
{
	Cmd* res_creation_cmd = m_ResourcesCommandBuffer.AppendCommand<Cmd>(cmd);
	res_creation_cmd->resource = ResourcesManager::GetGRM().Create<typename Cmd::Resource>(out_id_ptr);
	res_creation_cmd->settings = settings;
	return res_creation_cmd;
}

FORCEINLINE void RenderResourceContext::Update()
{
	m_ResourcesCommandBuffer.Submit();
}

TRE_NS_END