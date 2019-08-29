#pragma once

#include "Core/Misc/Defines/Common.hpp"
#include "RenderEngine/Renderer/Common/Common.hpp"
#include "RenderEngine/Renderer/Backend/CommandBuffer/RenderCommandBucket/RenderCommandBucket.hpp"
#include "RenderEngine/Renderer/Backend/CommandBuffer/ResourcesCommandBucket/ResourcesCommandBucket.hpp"
#include <RenderEngine/Renderer/Backend/CommandBuffer/FrameBufferCommandBucket/FramebufferCommandBucket.hpp>

TRE_NS_START

class IPrimitiveMesh;
class Scene;

class IRenderer
{
public:
    typedef RenderSettings::RenderCmdBuffer		 RenderCmdBuffer     ; 
	typedef RenderSettings::FramebufferCmdBuffer FramebufferCmdBuffer;

public:
    virtual void Render() = 0;

    virtual void PreRender() = 0;

    virtual void PostRender() = 0;

    virtual void Init() = 0;

    RenderCmdBuffer& GetRenderCommandBuffer() { return m_RenderCommandBuffer; }

	FramebufferCmdBuffer& GetFramebufferCommandBuffer() { return m_FramebufferCommandBuffer; }

    const RenderCmdBuffer& GetRenderCommandBuffer() const { return m_RenderCommandBuffer; }

	const FramebufferCmdBuffer& GetFramebufferCommandBuffer() const { return m_FramebufferCommandBuffer; }

protected:
    RenderCmdBuffer m_RenderCommandBuffer;
	FramebufferCmdBuffer m_FramebufferCommandBuffer;
};

TRE_NS_END