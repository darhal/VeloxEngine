#pragma once

#include <Core/Misc/Defines/Common.hpp>
#include <Renderer/Backend/CommandBuffer/CommandBuffer.hpp>
#include <Renderer/Mesh/IPrimitiveMesh/IPrimitiveMesh.hpp>
#include <Renderer/RenderPipline/Renderer/IRenderer.hpp>

TRE_NS_START

class ForwardPlusRenderer : public IRenderer
{
public:
	enum {
		DPETH_PASS = 0,
		LIGHT_CULLING_PASS = 1,
		LIGHT_ACCUM = 2,
	};
public:
	ForwardPlusRenderer();

	void Initialize(uint32 screen_width, uint32 screen_height);

	void Draw(IPrimitiveMesh& mesh);

	void CullLights();

	void Render();
private:
	void SetupFramebuffers(uint32 screen_width, uint32 screen_height);

	void SetupCommandBuffer();

	void SetupShaders();
private:
	CommandBuffer m_CommandQueue;
	Vec<2, uint32, normal> m_WorkGroups;
	uint32 m_LightCount;
	VboID m_LightBuffer, m_VisisbleLightIndicesBuffer;
	ShaderID m_DepthShader, m_LightCull, m_LightAccum, m_HdrShader;
	FboID m_DepthMapFbo, m_HdrFbo;
	TextureID m_DepthMap;

	CONSTEXPR static uint32 MAX_LIGHTS = 1024;
	CONSTEXPR static uint32 GROUP_SIZE = 16;
};

TRE_NS_END
