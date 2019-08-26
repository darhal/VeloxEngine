#include "Renderer.hpp"

TRE_NS_START

void Renderer::Render(const Scene& scene)
{
    m_ResourcesCommandBuffer.Submit();
    m_RenderCommandBuffer.Submit(scene);
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
}

void Renderer::PreRender()
{
    
}

void Renderer::PostRender()
{

}

TRE_NS_END