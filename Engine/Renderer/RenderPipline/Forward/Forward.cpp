#include "Forward.hpp"
#include <Renderer/Backend/ResourcesManager/ResourcesManager.hpp>
#include <Renderer/Backend/Commands/Commands.hpp>
#include <RenderAPI/Shader/Shader.hpp>
#include <RenderAPI/Shader/ShaderProgram.hpp>
#include <Renderer/Backend/Lights/ILight/ILight.hpp>

TRE_NS_START

ForwardRenderer::ForwardRenderer()
{
}

void ForwardRenderer::Initialize(uint32 scr_width, uint32 scr_height)
{
	this->SetupCommandBuffer(scr_width, scr_height);
	this->SetupLightsBuffer();
}

void ForwardRenderer::SetupCommandBuffer(uint32 scr_width, uint32 scr_height)
{
	m_ShadowMappingShader = ResourcesManager::Instance().CreateResource<ShaderProgram>(ResourcesTypes::SHADER,
		Shader("res/Shader/Forward/shadow_depth_mapping.vs", ShaderType::VERTEX),
		Shader("res/Shader/Forward/shadow_depth_mapping.fs", ShaderType::FRAGMENT)
	);
	{
		ShaderProgram& shader = ResourcesManager::Instance().Get<ShaderProgram>(ResourcesTypes::SHADER, m_ShadowMappingShader);
		shader.LinkProgram();
		shader.Use();
		shader.AddUniform("u_ProjView");
		shader.AddUniform("u_Model");
		shader.AddUniform("u_ViewPosition");
	}

	ResourcesManager& manager = ResourcesManager::Instance();
	ContextOperationQueue& op_queue = manager.GetContextOperationsQueue();

	FboID main_fbo = manager.CreateResource<FBO>(ResourcesTypes::FBO);
	manager.Get<FBO>(ResourcesTypes::FBO, m_DepthFbo).Invalidate(); // Set it to zero

	CommandBucket& shadow_bucket = m_CommandQueue.CreateBucket();
	CommandBucket& bucket = m_CommandQueue.CreateBucket();
	bucket.GetProjectionMatrix() = mat4::perspective((float)bucket.GetCamera().Zoom, (float)scr_width / (float)scr_height, NEAR_PLANE, FAR_PLANE);

	m_DepthMap = manager.CreateResource<Texture>(ResourcesTypes::TEXTURE);
	Commands::CreateTextureCmd* cmd_tex = op_queue.SubmitCommand<Commands::CreateTextureCmd>();
	cmd_tex->texture = &manager.Get<Texture>(ResourcesTypes::TEXTURE, m_DepthMap);
	cmd_tex->settings = TextureSettings(
		TexTarget::TEX2D, scr_width, scr_height, NULL,
		{
			{TexParam::TEX_MIN_FILTER, TexFilter::NEAREST}, {TexParam::TEX_MAG_FILTER, TexFilter::NEAREST},
			{TexParam::TEX_WRAP_S, TexWrapping::CLAMP_BORDER}, {TexParam::TEX_WRAP_T, TexWrapping::CLAMP_BORDER}
		}, DataType::FLOAT, 0, TexInternalFormat::DepthComponent, TexFormat::DepthComponent, vec4(1.f, 1.f, 1.f, 1.f)
	);

	m_DepthFbo = manager.CreateResource<FBO>(ResourcesTypes::FBO);
	Commands::CreateFrameBufferCmd* cmd_fbo = op_queue.SubmitCommand<Commands::CreateFrameBufferCmd>();
	cmd_fbo->fbo = &manager.Get<FBO>(ResourcesTypes::FBO, m_DepthFbo);
	cmd_fbo->settings = FramebufferSettings(
		{ { cmd_tex->texture, FBOAttachement::DEPTH_ATTACH } },
		FBOTarget::FBO, NULL, FBOAttachement::DEPTH_ATTACH, {}, GL_NONE
	);

	// Setup shadow bucket
	Mat4f lightProjection = mat4::ortho(-10.f, 10.f, -10.f, 10.f, 1.f, 50.0f);
	Mat4f lightView = mat4::look_at(vec3(-2.0, 8.0, -1.0), vec3(0.f, 0.f, 0.f), vec3(0.0, 1.0, 0.0));
	shadow_bucket.GetProjectionMatrix() = lightProjection * lightView;
	shadow_bucket.GetRenderTarget().m_FboID = m_DepthFbo;
	shadow_bucket.SetOnBucketFlushCallback([](ResourcesManager& m, const RenderTarget& rt, const uint8* data) -> FBO& {
		ClearBuffers(Buffer::DEPTH);
		return CommandBucket::OnBucketFlushCallback(m, rt, data);
	});

	uint8* extra_data = bucket.GetExtraBuffer();
	new (extra_data) Mat4f(shadow_bucket.GetProjectionMatrix());
	new (extra_data + sizeof(Mat4f)) uint32(m_DepthMap); // Shadow Sampler
	// TODO: Change this to be on flush since this have doesnt depend on the key change (Or maybe not bcuz we get the shader at the last moment)
	bucket.SetOnKeyChangeCallback([](ResourcesManager& m, const BucketKey& key, const Mat4f& proj_view, const Mat4f& proj, const Camera& camera, const uint8* extra_data) -> ShaderProgram& {
		ShaderProgram& shader = CommandBucket::OnKeyChangeCallback(m, key, proj_view, proj, camera, extra_data);
		shader.SetMat4("u_LightSpaceMatrix", *reinterpret_cast<const Mat4f*>(extra_data));
		shader.SetInt("u_ShadowMap", 5);

		// Set shadow depth map:
		const uint32& depth_map_id = *reinterpret_cast<const uint32*>(extra_data + sizeof(Mat4f));
		Texture& shadow_map = m.Get<Texture>(ResourcesTypes::TEXTURE, depth_map_id);
		ActivateTexture(5);
		shadow_map.Bind();
		return shader;
	});

	// m_MeshSystem.SetCommandBucket(&bucket);
	// ResourcesManager::Instance().GetRenderWorld().GetSystsemList(SystemList::ACTIVE).AddSystem(&m_MeshSystem);
}

void ForwardRenderer::SetupLightsBuffer()
{
	ResourcesManager& manager = ResourcesManager::Instance();
	m_LightBuffer = manager.CreateResource<VBO>(ResourcesTypes::VBO);
	Commands::CreateVBOCmd* cmd = manager.GetContextOperationsQueue().SubmitCommand<Commands::CreateVBOCmd>();
	cmd->vbo = &manager.Get<VBO>(ResourcesTypes::VBO, m_LightBuffer);
	cmd->settings = VertexBufferSettings(sizeof(ILight) * MAX_LIGHTS + sizeof(uint32), BufferTarget::UNIFORM_BUFFER, BufferUsage::STATIC_DRAW);

	Commands::BindBufferRangeCmd* bind_cmd = manager.GetContextOperationsQueue().SubmitCommand<Commands::BindBufferRangeCmd>();
	bind_cmd->vbo = cmd->vbo;
	bind_cmd->binding_point = 0;
	bind_cmd->offset = 0;
	bind_cmd->size = sizeof(ILight) * MAX_LIGHTS + sizeof(uint32);

	m_LightSystem.m_LightVbo = cmd->vbo;
	m_LightSystem.m_MaxLight = MAX_LIGHTS;
}

void ForwardRenderer::Draw(IPrimitiveMesh& mesh)
{
	// Main Pass:
	mesh.Submit(m_CommandQueue.GetCommandBucker(MAIN_PASS));
	mesh.Submit(m_CommandQueue.GetCommandBucker(SHADOW_PASS), m_ShadowMappingShader, -1);
}

void ForwardRenderer::Render()
{
	m_CommandQueue.GetCommandBucker(MAIN_PASS).Finalize();
	m_CommandQueue.DispatchCommands();
}

TRE_NS_END
