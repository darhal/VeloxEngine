#include "Renderer.hpp"
#include <RenderEngine/Mesh/IPrimitiveMesh/IPrimitiveMesh.hpp>

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
}

void Renderer::PreRender()
{
    
}

void Renderer::PostRender()
{

}

void SubmitToFBO(IPrimitiveMesh* mesh)
{

}

TRE_NS_END