#pragma once

#include <Core/Misc/Defines/Common.hpp>
#include "RenderEngine/Renderer/Frontend/Renderer/Renderer.hpp"

TRE_NS_START

class RenderManager
{
public:
    static Renderer& GetRenderer();

    static RenderManager& GetInstance();
    
private:
    RenderManager();

    Renderer m_Renderer;

    static RenderManager s_ManagerInstance;
};

TRE_NS_END