#include "Forward.hpp"
#include <Renderer/Backend/ResourcesManager/ResourcesManager.hpp>
#include <Renderer/Backend/Commands/Commands.hpp>
#include <RenderAPI/Shader/Shader.hpp>
#include <RenderAPI/Shader/ShaderProgram.hpp>
#include <Renderer/Backend/Lights/ILight/ILight.hpp>
#include <Renderer/Backend/Lights/DirectionalLight/DirectionalLight.hpp>

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
}

void ForwardRenderer::SetupLightsBuffer()
{
	ResourcesManager& manager = ResourcesManager::Instance();
	m_LightBuffer = manager.CreateResource<VBO>(ResourcesTypes::VBO);
	Commands::CreateVBO* cmd = manager.GetContextOperationsQueue().SubmitCommand<Commands::CreateVBO>();
	cmd->vbo = &manager.Get<VBO>(ResourcesTypes::VBO, m_LightBuffer);
	cmd->settings = VertexBufferSettings(sizeof(ILight) * MAX_LIGHTS + sizeof(uint32), BufferTarget::UNIFORM_BUFFER, BufferUsage::STATIC_DRAW);

	Commands::BindBufferRange* bind_cmd = manager.GetContextOperationsQueue().SubmitCommand<Commands::BindBufferRange>();
	bind_cmd->vbo = cmd->vbo;
	bind_cmd->binding_point = 0;
	bind_cmd->offset = 0;
	bind_cmd->size = sizeof(ILight) * MAX_LIGHTS + sizeof(uint32);

	{
		Commands::EditSubBuffer* edit_sub_buff = manager.GetContextOperationsQueue().SubmitCommand<Commands::EditSubBuffer>();
		edit_sub_buff->vbo = cmd->vbo;
		DirectionalLight* light = new DirectionalLight();
		light->SetDirection(vec3(0.f, -0.5f, 0.f));
		light->SetLightColor(vec3(1.0f, 1.0f, 0.8f));
		edit_sub_buff->data = &light->GetLightMatrix();
		edit_sub_buff->offset = 0;
		edit_sub_buff->size = sizeof(Mat4f);
	}
	{
		Commands::EditSubBuffer* edit_sub_buff = manager.GetContextOperationsQueue().SubmitCommand<Commands::EditSubBuffer>();
		edit_sub_buff->vbo = cmd->vbo;
		uint32* count = new uint32(1);
		edit_sub_buff->data = count;
		edit_sub_buff->offset = sizeof(ILight) * MAX_LIGHTS;
		edit_sub_buff->size = sizeof(uint32);
	}
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
