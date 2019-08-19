#pragma once

#include "Core/Misc/Defines/Common.hpp"
#include "RenderEngine/Renderer/Common/Common.hpp"
#include "RenderEngine/Renderer/Backend/CommandBuffer/RenderCommandBucket/RenderCommandBucket.hpp"
#include "RenderEngine/Renderer/Backend/CommandBuffer/ResourcesCommandBucket/ResourcesCommandBucket.hpp"

TRE_NS_START

class IPrimitiveMesh;
class Scene;

class IRenderer
{
public:
    typedef RenderSettings::RenderCmdBuffer RenderCmdBuffer; 
    typedef RenderSettings::ResourcesCmdBuffer ResourcesCmdBuffer;
    
public:
    virtual void Render(const Scene& scene) = 0;

    virtual void PreRender() = 0;

    virtual void PostRender() = 0;

    virtual void Init() = 0;

    RenderCmdBuffer& GetRenderCommandBuffer() { return m_RenderCommandBuffer; }

    ResourcesCmdBuffer& GetResourcesCommandBuffer() { return m_ResourcesCommandBuffer; }

    const RenderCmdBuffer& GetRenderCommandBuffer() const { return m_RenderCommandBuffer; }

    const ResourcesCmdBuffer& GetResourcesCommandBuffer() const { return m_ResourcesCommandBuffer; }

protected:
    RenderCmdBuffer m_RenderCommandBuffer;
    ResourcesCmdBuffer m_ResourcesCommandBuffer;
};

TRE_NS_END