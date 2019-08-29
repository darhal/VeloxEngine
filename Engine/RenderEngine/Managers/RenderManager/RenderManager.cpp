#include "RenderManager.hpp"
#include "RenderEngine/Renderer/Frontend/IRenderer/IRenderer.hpp"

TRE_NS_START

RenderManager RenderManager::s_ManagerInstance;

RenderManager::RenderManager()
{
	m_Renderer.Init();
}

Renderer& RenderManager::GetRenderer()
{
    return RenderManager::GetInstance().m_Renderer;
}

RenderResourceContext& RenderManager::GetRenderResourceContext()
{
	return RenderManager::GetInstance().m_RRC;
}

RenderResourceContext& RenderManager::GetRRC()
{
	return RenderManager::GetRenderResourceContext();
}

RenderManager& RenderManager::GetInstance()
{
    return RenderManager::s_ManagerInstance;
}

void RenderManager::Update()
{
	GetRRC().GetResourcesCommandBuffer().Submit();
	GetRenderer().Render();
}

TRE_NS_END