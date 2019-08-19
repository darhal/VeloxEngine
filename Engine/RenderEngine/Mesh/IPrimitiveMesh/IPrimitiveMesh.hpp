#pragma once

#include "Core/Misc/Defines/Common.hpp"
#include "RenderEngine/Renderer/Frontend/IRenderer/IRenderer.hpp"

TRE_NS_START

class IPrimitiveMesh
{
public:
    typedef IRenderer::RenderCmdBuffer RenderCommandBuffer;

    virtual void Submit(RenderCommandBuffer& CmdBucket, const Vec3f& CameraPosition) = 0; // Shouldd be pure virtual
private:
    
};

TRE_NS_END