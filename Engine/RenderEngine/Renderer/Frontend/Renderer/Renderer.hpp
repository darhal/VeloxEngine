#pragma once

#include "Core/Misc/Defines/Common.hpp"
#include "RenderEngine/Renderer/Frontend/IRenderer/IRenderer.hpp"
#include <RenderEngine/Scenegraph/Scene/Scene.hpp>

TRE_NS_START

class Renderer : public IRenderer
{
public:
    Renderer() = default;

    void PreRender() override;

    void Render() override;

    void PostRender() override;

    void Init() override;

	Scene& GetScene() { return scene; }

	const MaterialID& GetShadowMaterialID() const { return m_ShadowMaterial; }

	const TextureID& GetShadowMap() const { return m_ShadowMap; }

	vec3 LightPos = vec3(-0.2f, -1.0f, -0.3f);
	mat4 lightProjection = mat4::ortho(-10.f, 10.f, -10.f, 10.f, 1.f, 7.5f);
	mat4 lightView = mat4::look_at(LightPos, vec3(0.f, 0.f, 0.f), vec3(0.f, 1.f, 0.f));
private:
	Scene scene;
	MaterialID m_ShadowMaterial;
	TextureID m_ShadowMap;
};

TRE_NS_END