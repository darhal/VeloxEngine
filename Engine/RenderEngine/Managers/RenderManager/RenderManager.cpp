#include "RenderManager.hpp"
#include "RenderEngine/Renderer/Frontend/IRenderer/IRenderer.hpp"

TRE_NS_START

RenderManager RenderManager::s_ManagerInstance;

RenderManager::RenderManager()
{
	// m_Renderer.Init(); Dont init now till we have the context ready
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

void RenderManager::Init()
{
	GetRenderer().Init();
}

void RenderManager::Update()
{
	GetRRC().Update();
	GetRenderer().Render();
}

TRE_NS_END