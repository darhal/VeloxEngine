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
	CommandBucket& bucket = m_CommandQueue.CreateBucket();
	bucket.GetProjectionMatrix() = mat4::perspective((float)bucket.GetCamera().Zoom, (float)scr_width / (float)scr_height, NEAR_PLANE, FAR_PLANE);

	ResourcesManager& manager = ResourcesManager::Instance();
	ContextOperationQueue& op_queue = manager.GetContextOperationsQueue();
	CommandBucket& shadow_bucket = m_CommandQueue.CreateBucket();

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
}

void ForwardRenderer::Render()
{
	m_CommandQueue.GetCommandBucker(MAIN_PASS).Finalize();
	m_CommandQueue.DispatchCommands();
}

TRE_NS_END
