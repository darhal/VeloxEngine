#pragma once

#include <Core/Misc/Defines/Common.hpp>
#include "RenderEngine/Renderer/Frontend/Renderer/Renderer.hpp"
#include <RenderEngine/Renderer/Frontend/RenderResourceContext/RenderResourceContext.hpp>

TRE_NS_START

class RenderManager
{
public:
    static Renderer& GetRenderer();

	static RenderResourceContext& GetRenderResourceContext();

	static RenderResourceContext& GetRRC();

    static RenderManager& GetInstance();

	static void Update();
    
private:
    RenderManager();

    Renderer m_Renderer;

	RenderResourceContext m_RRC;
	
    static RenderManager s_ManagerInstance;
};

TRE_NS_END