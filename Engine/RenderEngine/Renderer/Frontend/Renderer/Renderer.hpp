#pragma once

#include "Core/Misc/Defines/Common.hpp"
#include "RenderEngine/Renderer/Frontend/IRenderer/IRenderer.hpp"

TRE_NS_START

class Renderer : public IRenderer
{
public:
    Renderer() = default;

    void PreRender() override;

    void Render() override;

    void PostRender() override;

    void Init() override;
private:
};

TRE_NS_END