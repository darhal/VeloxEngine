#include "ForwardPlus.hpp"
#include <Renderer/Backend/ResourcesManager/ResourcesManager.hpp>
#include <Renderer/Backend/Commands/Commands.hpp>
#include <RenderAPI/Shader/Shader.hpp>
#include <RenderAPI/Shader/ShaderProgram.hpp>

TRE_NS_START

ForwardPlusRenderer::ForwardPlusRenderer()
{
}

void ForwardPlusRenderer::Initialize(uint32 screen_width, uint32 screen_height)
{
	m_WorkGroups = Vec2<uint32>(
		(screen_width + (screen_width % GROUP_SIZE)) / GROUP_SIZE,
		(screen_height + (screen_height % GROUP_SIZE)) / GROUP_SIZE
	);
	uint32 number_of_tiles = m_WorkGroups.x * m_WorkGroups.y;

	ResourcesManager& manager = ResourcesManager::Instance();
	ContextOperationQueue& op_queue = manager.GetContextOperationsQueue();

	m_LightBuffer = manager.CreateResource<VBO>(ResourcesTypes::VBO);
	Commands::CreateVBOCmd* cmd = op_queue.SubmitCommand<Commands::CreateVBOCmd>();
	cmd->vbo = &manager.Get<VBO>(ResourcesTypes::VBO, m_LightBuffer);
	cmd->settings = VertexBufferSettings(MAX_LIGHTS * sizeof(Mat4f), BufferTarget::SHADER_STORAGE_BUFFER, BufferUsage::DYNAMIC_DRAW);

	m_VisisbleLightIndicesBuffer = manager.CreateResource<VBO>(ResourcesTypes::VBO);
	cmd = op_queue.SubmitCommand<Commands::CreateVBOCmd>();
	cmd->vbo = &manager.Get<VBO>(ResourcesTypes::VBO, m_VisisbleLightIndicesBuffer);
	cmd->settings = VertexBufferSettings(number_of_tiles * MAX_LIGHTS * sizeof(uint32), BufferTarget::SHADER_STORAGE_BUFFER, BufferUsage::STATIC_DRAW);

	this->SetupShaders();
	this->SetupFramebuffers(screen_width, screen_height);
	this->SetupCommandBuffer();
}

void ForwardPlusRenderer::SetupFramebuffers(uint32 screen_width, uint32 screen_height)
{
	ResourcesManager& manager = ResourcesManager::Instance();
	ContextOperationQueue& op_queue = manager.GetContextOperationsQueue();

	m_DepthMap = manager.CreateResource<Texture>(ResourcesTypes::TEXTURE);
	Commands::CreateTextureCmd* cmd_tex = op_queue.SubmitCommand<Commands::CreateTextureCmd>();
	cmd_tex->texture = &manager.Get<Texture>(ResourcesTypes::TEXTURE, m_DepthMap);
	cmd_tex->settings = TextureSettings(
		TexTarget::TEX2D, screen_width, screen_height, NULL,
		{
			{TexParam::TEX_MIN_FILTER, TexFilter::NEAREST}, {TexParam::TEX_MAG_FILTER, TexFilter::NEAREST},
			{TexParam::TEX_WRAP_S, TexWrapping::CLAMP_BORDER}, {TexParam::TEX_WRAP_T, TexWrapping::CLAMP_BORDER}
		}, DataType::FLOAT, 0, TexInternalFormat::DepthComponent, TexFormat::DepthComponent, vec4(1.f, 1.f, 1.f, 1.f)
	);

	m_DepthMapFbo = manager.CreateResource<FBO>(ResourcesTypes::FBO);
	Commands::CreateFrameBufferCmd* cmd_fbo = op_queue.SubmitCommand<Commands::CreateFrameBufferCmd>();
	cmd_fbo->fbo = &manager.Get<FBO>(ResourcesTypes::FBO, m_DepthMapFbo);
	cmd_fbo->settings = FramebufferSettings(
		{ { cmd_tex->texture, FBOAttachement::DEPTH_ATTACH } },
		FBOTarget::FBO, NULL, FBOAttachement::DEPTH_ATTACH, {}, GL_NONE
	);
}

void ForwardPlusRenderer::SetupShaders()
{
	m_DepthShader = ResourcesManager::Instance().CreateResource<ShaderProgram>(ResourcesTypes::SHADER,
		Shader("res/Shader/Forward+/depth.vert.glsl", ShaderType::VERTEX),
		Shader("res/Shader/Forward+/depth.frag.glsl", ShaderType::FRAGMENT)
		);
	{
		ShaderProgram& shader = ResourcesManager::Instance().Get<ShaderProgram>(ResourcesTypes::SHADER, m_DepthShader);
		shader.LinkProgram();
		shader.Use();
		shader.AddUniform("u_ProjView");
		shader.AddUniform("u_Model");
	}

	m_LightCull = ResourcesManager::Instance().CreateResource<ShaderProgram>(ResourcesTypes::SHADER,
		std::initializer_list<Shader>{ Shader("res/Shader/Forward+/light_culling.comp.glsl", ShaderType::COMPUTE) }
	);
	{
		ShaderProgram& shader = ResourcesManager::Instance().Get<ShaderProgram>(ResourcesTypes::SHADER, m_LightCull);
		shader.LinkProgram();
		shader.Use();
		shader.AddUniform("u_LightCount");
		shader.AddUniform("u_ScreenSize");
		shader.AddUniform("u_Projection");
		shader.AddUniform("u_View");
		shader.AddUniform("u_DepthMap");
	}

	m_LightAccum = ResourcesManager::Instance().CreateResource<ShaderProgram>(ResourcesTypes::SHADER,
		Shader("res/Shader/Forward+/light_accumulation.vert.glsl", ShaderType::VERTEX),
		Shader("res/Shader/Forward+/light_accumulation.frag.glsl", ShaderType::FRAGMENT)
		);
	{
		ShaderProgram& shader = ResourcesManager::Instance().Get<ShaderProgram>(ResourcesTypes::SHADER, m_LightAccum);
		shader.LinkProgram();
		shader.Use();
		shader.AddUniform("u_ProjView");
		shader.AddUniform("u_Model");
		shader.AddUniform("u_NumberOfTilesX");
		shader.AddUniform("u_ViewPosition");
	}

	ResourcesManager& manager = ResourcesManager::Instance();
	ContextOperationQueue& op_queue = manager.GetContextOperationsQueue();
}

void ForwardPlusRenderer::SetupCommandBuffer()
{
	ResourcesManager& manager = ResourcesManager::Instance();

	// These are later executed in this same order
	CommandBucket& depth_bucket = m_CommandQueue.CreateBucket();
	{
		RenderTarget& render_target = depth_bucket.GetRenderTarget();
		render_target.m_FboID = m_DepthMapFbo;
	}

	CommandBucket& light_culling_bucket = m_CommandQueue.CreateBucket();
	{
		RenderTarget& render_target = depth_bucket.GetRenderTarget();
		render_target.m_FboID = 0;
		light_culling_bucket.SetOnBucketFlushCallback(NULL);
		light_culling_bucket.SetOnKeyChangeCallback(NULL);
	}

	CommandBucket& light_accum_bucket = m_CommandQueue.CreateBucket();
	{
		RenderTarget& render_target = depth_bucket.GetRenderTarget();
		render_target.m_FboID = 0;
	}
}


void ForwardPlusRenderer::CullLights()
{
	ResourcesManager& manager = ResourcesManager::Instance();
    // Get our main and only packet
	CommandPacket& packet = m_CommandQueue.GetCommandBucker(LIGHT_CULLING_PASS).GetOrCreateCommandPacket(0);

	// Set Uniforms (projection, set view, set depthMap), bind bufferbase for light buffers, then finally dispatch compute
	// Submit commands in this orderer.
	Commands::UploadUniformsCmd* setup_uniforms_cmd = packet.SubmitCommand<Commands::UploadUniformsCmd>(0);
	setup_uniforms_cmd->shader = &manager.Get<ShaderProgram>(ResourcesTypes::SHADER, m_LightCull);
	setup_uniforms_cmd->m_Params = UniformsParams<int32>();
	setup_uniforms_cmd->m_Params.AddParameter<Mat4f>(setup_uniforms_cmd->shader->GetUniform("u_View").second, Mat4f());
	setup_uniforms_cmd->m_Params.AddParameter<Mat4f>(setup_uniforms_cmd->shader->GetUniform("u_Projection").second, Mat4f());
	setup_uniforms_cmd->m_Params.AddParameter<TextureID>(setup_uniforms_cmd->shader->GetUniform("u_DepthMap").second, m_DepthMap);
	setup_uniforms_cmd->m_Params.AddParameter<int32>(setup_uniforms_cmd->shader->GetUniform("u_LightCount").second, m_LightCount);
	// setup_uniforms_cmd->m_Params.AddParameter<Vec2<int32>>(setup_uniforms_cmd->shader->GetUniform("u_ScreenSize").second, m_DepthMap);

	Commands::BindBufferBaseCmd* bind_buffer_base = packet.SubmitCommand<Commands::BindBufferBaseCmd>(0);
	bind_buffer_base->vbo = &manager.Get<VBO>(ResourcesTypes::VBO, m_LightBuffer);
	bind_buffer_base->binding_point = 0;

	bind_buffer_base = packet.SubmitCommand<Commands::BindBufferBaseCmd>(0);
	bind_buffer_base->vbo = &manager.Get<VBO>(ResourcesTypes::VBO, m_VisisbleLightIndicesBuffer);
	bind_buffer_base->binding_point = 1;

	Commands::DispatchComputeCmd* dispatchcmd = packet.SubmitCommand<Commands::DispatchComputeCmd>(0);
	dispatchcmd->workGroupX = m_WorkGroups.x;
	dispatchcmd->workGroupY = m_WorkGroups.y;
	dispatchcmd->workGroupZ = 1;
	dispatchcmd->next_cmd = NULL;
}

void ForwardPlusRenderer::Draw(IPrimitiveMesh& mesh)
{
	// Depth Pass:
	mesh.Submit(m_CommandQueue.GetCommandBucker(DPETH_PASS), m_DepthShader);

	// Compute shader phase here (It's automatic)

	// Light Accumlation Pass:
	mesh.Submit(m_CommandQueue.GetCommandBucker(LIGHT_ACCUM), m_LightAccum);
}

void ForwardPlusRenderer::Render()
{
	m_CommandQueue.DispatchCommands();
}

TRE_NS_END