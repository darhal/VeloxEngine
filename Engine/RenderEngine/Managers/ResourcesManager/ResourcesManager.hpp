#pragma once

#include <Core/Misc/Defines/Common.hpp>
#include <RenderEngine/Managers/GraphicsResourcesManager/GRM.hpp>

TRE_NS_START

class ResourcesManager
{
public:
    static GRM& GetGRM();

    static ResourcesManager& GetInstance();
    
private:
    ResourcesManager();

    GRM m_GRM;

    static ResourcesManager s_ManagerInstance;
};

TRE_NS_END