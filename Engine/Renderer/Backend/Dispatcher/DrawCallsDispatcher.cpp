#include "DrawCallsDispatcher.hpp"
#include "RenderAPI/VertexArray/VAO.hpp"
#include "RenderAPI/VertexBuffer/VBO.hpp"
#include "RenderAPI/General/GLContext.hpp"
#include <RenderAPI/Shader/ShaderProgram.hpp>
#include "Renderer/Backend/Commands/Commands.hpp"
#include <Renderer/Backend/ResourcesManager/ResourcesManager.hpp>
#include <Renderer/Materials/Material.hpp>

TRE_NS_START

/************************************ Single Draw Commands ************************************/

void BackendDispatch::PreDrawCall(const void* data)
{
	const Commands::PreDrawCallCmd* real_data = reinterpret_cast<const Commands::PreDrawCallCmd*>(data);

	VAO& vao = ResourcesManager::Instance().Get<VAO>(ResourcesTypes::VAO, real_data->vao_id);
	vao.Bind();
	real_data->shader->SetMat4("u_Model", real_data->model);

	Cmd next_cmd = real_data->next_cmd;

	while (next_cmd) {
		const BackendDispatchFunction CommandFunction = Command::LoadBackendDispatchFunction(next_cmd);
		const void* command = Command::LoadCommand(next_cmd);
		CommandFunction(command);

		next_cmd = ((Commands::LinkCmd*)command)->next_cmd;
	}
}

void BackendDispatch::Draw(const void* data)
{
    const Commands::DrawCmd* real_data = reinterpret_cast<const Commands::DrawCmd*>(data);

	real_data->material->GetTechnique().UploadUnfiroms(*real_data->prepare_cmd->shader);
    DrawArrays(real_data->mode, real_data->start, real_data->end);
}

void BackendDispatch::DrawIndexed(const void* data)
{
    const Commands::DrawIndexedCmd* real_data = reinterpret_cast<const Commands::DrawIndexedCmd*>(data);

	real_data->material->GetTechnique().UploadUnfiroms(*real_data->prepare_cmd->shader);
    DrawElements(real_data->mode, real_data->type, real_data->count, real_data->offset);
}

/************************************ Instanced Draw Commands ************************************/

void BackendDispatch::InstancedPreDrawCall(const void* data)
{
	const Commands::PreInstancedDrawCallCmd* real_data = reinterpret_cast<const Commands::PreInstancedDrawCallCmd*>(data);

	VAO& vao = ResourcesManager::Instance().Get<VAO>(ResourcesTypes::VAO, real_data->vao_id);
	vao.Bind();

	Cmd next_cmd = real_data->next_cmd;

	while (next_cmd) {
		const BackendDispatchFunction CommandFunction = Command::LoadBackendDispatchFunction(next_cmd);
		const void* command = Command::LoadCommand(next_cmd);
		CommandFunction(command);

		next_cmd = ((Commands::LinkCmd*)command)->next_cmd;
	}
}

void BackendDispatch::InstancedDraw(const void* data)
{
	const Commands::InstancedDrawCmd* real_data = reinterpret_cast<const Commands::InstancedDrawCmd*>(data);

	real_data->material->GetTechnique().UploadUnfiroms(*real_data->prepare_cmd->shader);
	DrawArraysInstanced(real_data->mode, real_data->start, real_data->end, real_data->instance_count);
}

void BackendDispatch::InstancedDrawIndexed(const void* data)
{
	const Commands::InstancedDrawIndexedCmd* real_data = reinterpret_cast<const Commands::InstancedDrawIndexedCmd*>(data);

	real_data->material->GetTechnique().UploadUnfiroms(*real_data->prepare_cmd->shader);
	DrawElementsInstanced(real_data->mode, real_data->type, real_data->count, real_data->offset, real_data->instance_count);
}

/************************************ RESOURCES CREATION COMMANDS ************************************/

void BackendDispatch::CreateVAO(const void* data)
{
	const Commands::CreateVAOCmd* real_data = reinterpret_cast<const Commands::CreateVAOCmd*>(data);
	VAO* modelVAO = real_data->vao;
	const VertexSettings& settings = real_data->settings;

	modelVAO->Generate();
	modelVAO->Bind();

	const auto& vert_data = settings.vertices_data;
	const uint32 vert_len = (uint32) vert_data.Length();

	for (const VertexSettings::VertexBufferData& data : vert_data) {
		VBO& vbo = *data.vbo;
		vbo.Generate(data.buffer_settings.target);
		vbo.Bind();
		vbo.FillData(data.buffer_settings.data, data.buffer_settings.total_size, data.buffer_settings.usage);
		// ::operator delete(data.data);
	}

	uint32 index = 0;
	for (const VertexSettings::VertexAttribute& attribute : settings.attributes) {
		VBO& vbo = *vert_data[attribute.vbo_index].vbo;
		vbo.Bind();
		modelVAO->BindAttribute<float>(attribute.attrib_index, vbo, attribute.data_type, attribute.size, attribute.stride, attribute.offset);
		modelVAO->SetVertextAttribDivisor(attribute.attrib_index, attribute.divisor);
		index++;
	}
}

void BackendDispatch::CreateIndexBuffer(const void* data)
{
    const Commands::CreateIndexBufferCmd* real_data = reinterpret_cast<const Commands::CreateIndexBufferCmd*>(data);

    VAO& modelVAO = *real_data->vao;
	const VertexSettings::VertexBufferData& indices_data = real_data->settings;
	VBO& indexVBO = *indices_data.vbo;

	// Set up indices
	indexVBO.Generate(BufferTarget::ELEMENT_ARRAY_BUFFER);
	indexVBO.Bind();
	indexVBO.FillData(indices_data.buffer_settings.data, indices_data.buffer_settings.total_size);
	modelVAO.Unbind();
	indexVBO.Unbind();

	// ::operator delete(indices_data.data);
}

void BackendDispatch::CreateVBO(const void* data)
{
	const Commands::CreateVBOCmd* real_data = reinterpret_cast<const Commands::CreateVBOCmd*>(data);

	VBO& vbo = *real_data->vbo;
	const VertexBufferSettings& settings = real_data->settings;

	// Set up VBO
	vbo.Generate(settings.target);
	vbo.Bind();
	vbo.FillData(settings.data, settings.total_size, settings.usage);
	vbo.Unbind();

	// ::operator delete(settings.data);
}

void BackendDispatch::CreateTexture(const void* data)
{
    const Commands::CreateTextureCmd* real_data = reinterpret_cast<const Commands::CreateTextureCmd*>(data);
    Texture* tex = real_data->texture;
    const TextureSettings& settings = real_data->settings;

    tex->Generate(settings);

	if (settings.img_data != NULL) {
		::operator delete(settings.img_data);
	}
}

void BackendDispatch::CreateFrameBuffer(const void * data)
{
	const Commands::CreateFrameBufferCmd* real_data = reinterpret_cast<const Commands::CreateFrameBufferCmd*>(data);
	Framebuffer* fbo = real_data->fbo;
	const FramebufferSettings& settings = real_data->settings;

	fbo->Generate(settings);
}

void BackendDispatch::CreateRenderBuffer(const void * data)
{
	const Commands::CreateRenderBufferCmd* real_data = reinterpret_cast<const Commands::CreateRenderBufferCmd*>(data);
	Renderbuffer* rbo = real_data->rbo;
	const RenderbufferSettings& settings = real_data->settings;

	rbo->Generate(settings);
}

/************************************ Misc Commands ************************************/

void BackendDispatch::EditSubBuffer(const void* data)
{
	const Commands::EditSubBufferCmd* real_data = reinterpret_cast<const Commands::EditSubBufferCmd*>(data);

	real_data->vbo->Bind();
	Call_GL(glBufferSubData(real_data->vbo->GetBindingTarget(), real_data->offset, real_data->size, real_data->data));
}

void BackendDispatch::DispatchCompute(const void* data)
{
	const Commands::DispatchComputeCmd* real_data = reinterpret_cast<const Commands::DispatchComputeCmd*>(data);

	Cmd next_cmd = real_data->next_cmd;

	while (next_cmd) {
		const BackendDispatchFunction CommandFunction = Command::LoadBackendDispatchFunction(next_cmd);
		const void* command = Command::LoadCommand(next_cmd);
		CommandFunction(command);

		next_cmd = ((Commands::LinkCmd*)command)->next_cmd;
	}

	Call_GL(glDispatchCompute(real_data->workGroupX, real_data->workGroupY, real_data->workGroupZ));
}

void BackendDispatch::UploadUniforms(const void* data)
{
	const Commands::UploadUniformsCmd* real_data = reinterpret_cast<const Commands::UploadUniformsCmd*>(data);

	real_data->shader->Bind();
	real_data->m_Params.UploadUnfiroms(*real_data->shader);
}

void BackendDispatch::BindBufferBase(const void* data)
{
	const Commands::BindBufferBaseCmd* real_data = reinterpret_cast<const Commands::BindBufferBaseCmd*>(data);

	Call_GL(glBindBufferBase(real_data->vbo->GetBindingTarget(), real_data->binding_point, real_data->vbo->GetID()););
}

void BackendDispatch::BindBufferRange(const void* data)
{
	const Commands::BindBufferRangeCmd* real_data = reinterpret_cast<const Commands::BindBufferRangeCmd*>(data);

	Call_GL(glBindBufferRange(real_data->vbo->GetBindingTarget(), real_data->binding_point, real_data->vbo->GetID(), real_data->offset, real_data->size););
}

void BackendDispatch::MapBufferCmd(const void* data)
{
	const Commands::MapBufferCmd* real_data = reinterpret_cast<const Commands::MapBufferCmd*>(data);

	void* mapped_data = glMapBuffer(real_data->target, real_data->access);
	real_data->callback(mapped_data, real_data);
	glUnmapBuffer(real_data->target);
}

void BackendDispatch::CallFunctionCmd(const void* data)
{
	const Commands::CallFunctionCmd* real_data = reinterpret_cast<const Commands::CallFunctionCmd*>(data);

	real_data->callback(real_data->data);
	// TODO: Free real_data->data;
}

TRE_NS_END