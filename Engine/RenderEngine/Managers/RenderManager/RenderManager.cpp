#include "RenderManager.hpp"
#include "RenderEngine/Renderer/Frontend/IRenderer/IRenderer.hpp"

TRE_NS_START

RenderManager RenderManager::s_ManagerInstance;

RenderManager::RenderManager()
{
}

Renderer& RenderManager::GetRenderer()
{
    return RenderManager::GetInstance().m_Renderer;
}

RenderManager& RenderManager::GetInstance()
{
    return RenderManager::s_ManagerInstance;
}

TRE_NS_END