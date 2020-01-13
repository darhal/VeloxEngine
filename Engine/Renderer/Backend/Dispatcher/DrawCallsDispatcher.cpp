#include "DrawCallsDispatcher.hpp"
#include "RenderAPI/VertexArray/VAO.hpp"
#include "RenderAPI/VertexBuffer/VBO.hpp"
#include "RenderAPI/General/GLContext.hpp"
#include "Renderer/Backend/Commands/Commands.hpp"

TRE_NS_START

void BackendDispatch::Draw(const void* data)
{
    const Commands::DrawCmd* real_data = reinterpret_cast<const Commands::DrawCmd*>(data);
    DrawArrays(real_data->mode, real_data->start, real_data->end);
}

void BackendDispatch::DrawIndexed(const void* data)
{
    const Commands::DrawIndexedCmd* real_data = reinterpret_cast<const Commands::DrawIndexedCmd*>(data);
    DrawElements(real_data->mode, real_data->type, real_data->count, real_data->offset);
}

void BackendDispatch::CreateVAO(const void* data)
{
	const Commands::CreateVAO* real_data = reinterpret_cast<const Commands::CreateVAO*>(data);
	VAO* modelVAO = real_data->vao;
	const VertexSettings& settings = real_data->settings;

	modelVAO->Generate();
	modelVAO->Bind();

	const auto& vert_data = settings.vertices_data;
	const uint32 vert_len = (uint32) vert_data.Length();

	for (const VertexSettings::VertexBufferData& data : vert_data) {
		VBO& vbo = *data.vbo;
		vbo.Generate(BufferTarget::ARRAY_BUFFER);
		vbo.Bind();
		vbo.FillData(data.data, data.elements_count * data.size);
		// ::operator delete(data.data);
	}

	uint32 index = 0;
	for (const VertexSettings::VertexAttribute& attribute : settings.attributes) {
		VBO& vbo = *vert_data[index % vert_len].vbo;
		vbo.Bind();
		modelVAO->BindAttribute<float>(attribute.attrib_index, vbo, attribute.data_type, attribute.size, attribute.stride, attribute.offset);
		index++;
	}
}

void BackendDispatch::CreateIndexBuffer(const void* data)
{
    const Commands::CreateIndexBuffer* real_data = reinterpret_cast<const Commands::CreateIndexBuffer*>(data);

    VAO& modelVAO = *real_data->vao;
	const VertexSettings::VertexBufferData& indices_data = real_data->settings;
	VBO& indexVBO = *indices_data.vbo;

	// Set up indices
	indexVBO.Generate(BufferTarget::ELEMENT_ARRAY_BUFFER);
	indexVBO.Use();
	indexVBO.FillData(indices_data.data, indices_data.elements_count * indices_data.size);
	modelVAO.Unuse();
	indexVBO.Use();

	// ::operator delete(indices_data.data);
}

void BackendDispatch::CreateTexture(const void* data)
{
    const Commands::CreateTexture* real_data = reinterpret_cast<const Commands::CreateTexture*>(data);
    Texture* tex = real_data->texture;
    const TextureSettings& settings = real_data->settings;

    tex->Generate(settings);

	if (settings.img_data != NULL) {
		::operator delete(settings.img_data);
	}
}

void BackendDispatch::CreateFrameBuffer(const void * data)
{
	const Commands::CreateFrameBuffer* real_data = reinterpret_cast<const Commands::CreateFrameBuffer*>(data);
	Framebuffer* fbo = real_data->fbo;
	const FramebufferSettings& settings = real_data->settings;

	fbo->Generate(settings);
}

void BackendDispatch::CreateRenderBuffer(const void * data)
{
	const Commands::CreateRenderBuffer* real_data = reinterpret_cast<const Commands::CreateRenderBuffer*>(data);
	Renderbuffer* rbo = real_data->rbo;
	const RenderbufferSettings& settings = real_data->settings;

	rbo->Generate(settings);
}

TRE_NS_END