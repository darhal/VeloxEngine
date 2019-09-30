#include "Renderer.hpp"
#include <RenderEngine/Mesh/IPrimitiveMesh/IPrimitiveMesh.hpp>
#include <RenderEngine/Managers/RenderManager/RenderManager.hpp>
#include <RenderEngine/Mesh/StaticMesh/StaticMesh.hpp>
#include <RenderEngine/Mesh/ModelLoader/ModelLoader.hpp>

TRE_NS_START

void Renderer::Render()
{
    m_RenderCommandBuffer.Submit();
	m_FramebufferCommandBuffer.Submit();
}

void Renderer::Init()
{
    /*VBO vertexUBO(BufferTarget::UNIFORM_BUFFER);
	vertexUBO.FillData(NULL, 2 * sizeof(mat4));
	vertexUBO.Unbind();

    for (auto& shader_id : ResourcesManager::GetGRM().GetResourceContainer<ShaderProgram>()) {
        ShaderProgram& shader = shader_id.second;
        shader.SetUniformBlockBinding("VertexBlock", 0);
		shader.BindBufferBase(vertexUBO, 0);
    }*/

	const unsigned int SCR_WIDTH = 1920 / 2;
	const unsigned int SCR_HEIGHT = 1080 / 2;
	const unsigned int SHADOW_WIDTH = 1024;
	const unsigned int SHADOW_HEIGHT = 1024;
	auto& rrc = RenderManager::GetRRC();
	
	RenderManager::GetRenderer().GetRenderCommandBuffer().PopRenderTarget();
	/*AbstractMaterial abst_mat;
	//abst_mat.GetParametres().AddParameter<mat4>("lightSpaceMatrix", lightProjection * lightView);
	m_ShadowMaterial = ResourcesManager::GetGRM().Generate<Material>(abst_mat, 3); // the second arg is shadow shader id

	RenderManager::GetRenderer().GetRenderCommandBuffer().PopRenderTarget();
	// Shadows framebuffer.
	{
		FboID fbo_id = 0;
		auto tex_settings = TextureSettings(TexTarget::TEX2D, SHADOW_WIDTH, SHADOW_HEIGHT, NULL,
			Vector<TexParamConfig>{
				{ TexParam::TEX_MIN_FILTER, TexFilter::NEAREST },
				{ TexParam::TEX_MAG_FILTER, TexFilter::NEAREST },
				{ TexParam::TEX_WRAP_S, TexWrapping::CLAMP_BORDER },
				{ TexParam::TEX_WRAP_T, TexWrapping::CLAMP_BORDER },
			}, DataType::FLOAT, 0, TexInternalFormat::DepthComponent, TexFormat::DepthComponent, vec4(1.f, 1.f, 1.f, 1.f)
		);
		auto tex_cmd = rrc.CreateResource<Commands::CreateTexture>(&m_ShadowMap, tex_settings);
		FramebufferSettings::TextureAttachement tex_attach{ tex_cmd->texture, FBOAttachement::DEPTH_ATTACH };
		auto fbo_cmd = rrc.CreateResourceAfter<Commands::CreateFrameBuffer>(
			tex_cmd, &fbo_id, FramebufferSettings({ tex_attach }, FBOTarget::FBO, NULL, FBOAttachement::NONE, { FBOColourBuffer::NONE }, FBOColourBuffer::NONE)
		);

		RenderManager::GetRenderer().GetRenderCommandBuffer().PushRenderTarget(RenderTarget(fbo_id, SHADOW_WIDTH, SHADOW_HEIGHT, &lightProjection, &lightView));
	}*/

	// Post-processing framebuffer.
	{

		float quadVertices[] = { // positions
			// positions
			-1.0f, 1.0f, 0.f,
			-1.0f, -1.0f, 0.f,
			1.0f, -1.0f, 0.f,
			-1.0f, 1.0f, 0.f,
			1.0f, -1.0f, 0.f,
			1.0f,  1.0f, 0.f,
		};
		float quadTexCoords[] = { // texture Coords
			0.0f, 1.0f,
			0.0f, 0.0f,
			1.0f, 0.0f,
			0.0f, 1.0f,
			1.0f, 0.0f,
			1.0f, 1.0f
		};
		uint32 indices[] = { 0, 1, 2, 3, 4, 5 };

		StaticMesh* quad = new StaticMesh();
		ModelSettings settings;
		settings.vertices = quadVertices;
		settings.vertexSize = ARRAY_SIZE(quadVertices);
		settings.textures = quadTexCoords;
		settings.textureSize = ARRAY_SIZE(quadTexCoords);
		settings.indices = indices;
		settings.indexSize = ARRAY_SIZE(indices);
		settings.CopyData();

		ModelLoader loader(settings);
		loader.GetMaterials().PopBack();
		AbstractMaterial* abst_mat = new AbstractMaterial();
		abst_mat->GetRenderStates().depth_enabled = false;

		// Creating a frame buffer.
		TextureID tex_id = 0; FboID fbo_id = 0; RboID rbo_id = 0;
		auto tex_settings = TextureSettings(TexTarget::TEX2D, SCR_WIDTH, SCR_HEIGHT, NULL,
			Vector<TexParamConfig>{
				{ TexParam::TEX_MIN_FILTER, TexFilter::LINEAR },
				{ TexParam::TEX_MAG_FILTER, TexFilter::LINEAR }
			}
		);
		auto tex_cmd = rrc.CreateResource<Commands::CreateTexture>(&tex_id, tex_settings);
		auto rbo_cmd = rrc.CreateResourceAfter<Commands::CreateRenderBuffer>(tex_cmd, NULL, RenderbufferSettings(SCR_WIDTH, SCR_HEIGHT));
		auto fbo_cmd = rrc.CreateResourceAfter<Commands::CreateFrameBuffer>(rbo_cmd, &fbo_id, FramebufferSettings({ tex_cmd->texture }, FBOTarget::FBO, rbo_cmd->rbo));

		ShaderID shaderID = 2; // for depth testing
		abst_mat->GetParametres().AddParameter<TextureID>("screenTexture", tex_id);
		//abst_mat->GetParametres().AddParameter<TextureID>("depthMap", RenderManager::GetRenderer().GetShadowMap());
		//abst_mat->GetParametres().AddParameter<float>("near_plane", 1.0f);
		//abst_mat->GetParametres().AddParameter<float>("far_plane", 10.f);
		loader.GetMaterials().EmplaceBack(*abst_mat, loader.GetVertexCount());
		loader.ProcessData(*quad, shaderID);

		// Setting up render targets.
		RenderManager::GetRenderer().GetRenderCommandBuffer().PushRenderTarget(RenderTarget(fbo_id, SCR_WIDTH, SCR_HEIGHT, scene.GetProjectionMatrix(), &scene.GetCurrentCamera()->GetViewMatrix()));

		Renderer::FramebufferCmdBuffer::FrameBufferPiriority f;
		RenderTarget* rt = ResourcesManager::GetGRM().Create<RenderTarget>(f.render_target_id);
		rt->m_Width = SCR_WIDTH;
		rt->m_Height = SCR_HEIGHT;

		quad->Submit(RenderManager::GetRenderer().GetFramebufferCommandBuffer(), f.render_target_id);
	}

	RenderManager::GetRenderer().GetFramebufferCommandBuffer().SwapCmdBuffer();
}

void Renderer::PreRender()
{
    
}

void Renderer::PostRender()
{

}

TRE_NS_END