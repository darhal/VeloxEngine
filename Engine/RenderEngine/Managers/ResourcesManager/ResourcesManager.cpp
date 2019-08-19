#include "ResourcesManager.hpp"

TRE_NS_START

ResourcesManager ResourcesManager::s_ManagerInstance = ResourcesManager();

ResourcesManager::ResourcesManager()
{
    RenderSettings::DEFAULT_STATE_HASH = m_GRM.AddState(RenderSettings::DEFAULT_STATE);
    RenderSettings::DEFAULT_FRAMEBUFFER = m_GRM.Generate<FBO>();
}

GRM& ResourcesManager::GetGRM()
{
    return ResourcesManager::GetInstance().m_GRM;
}

ResourcesManager& ResourcesManager::GetInstance()
{
    return ResourcesManager::s_ManagerInstance;
}

TRE_NS_END