#pragma once

#include "Core/Misc/Defines/Common.hpp"
#include "RenderEngine/Renderer/Frontend/IRenderer/IRenderer.hpp"
#include <RenderEngine/Renderer/Backend/CommandBuffer/FrameBufferCommandBucket/FramebufferCommandBucket.hpp>

TRE_NS_START

class IPrimitiveMesh
{
public:
    typedef IRenderer::RenderCmdBuffer RenderCommandBuffer;
	typedef IRenderer::FramebufferCmdBuffer FramebufferCommandBuffer;

    virtual void Submit(RenderCommandBuffer& CmdBucket, const Vec3f& CameraPosition) = 0; // Should be pure virtual

	virtual void Submit(FramebufferCommandBuffer& CmdBucket, RenderTarget* render_target, FramebufferCommandBuffer::FrameBufferPiriority::Piroirty_t piroirty) = 0; // Should be pure virtual
private:
    
};

TRE_NS_END