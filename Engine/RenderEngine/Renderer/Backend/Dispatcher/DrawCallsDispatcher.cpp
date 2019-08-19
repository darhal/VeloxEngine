#include "DrawCallsDispatcher.hpp"
#include "RenderAPI/VertexArray/VAO.hpp"
#include "RenderAPI/VertexBuffer/VBO.hpp"
#include "RenderAPI/General/GLContext.hpp"
#include "RenderEngine/Renderer/Backend/Commands/Commands.hpp"
#include "RenderEngine/Managers/ResourcesManager/ResourcesManager.hpp"
#include "RenderEngine/Mesh/ModelLoader/ModelLoader.hpp"

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

void BackendDispatch::GenerateVAO(const void* data)
{
    const Commands::GenerateVAOCmd* real_data = reinterpret_cast<const Commands::GenerateVAOCmd*>(data);
    VAO* modelVAO = real_data->m_VAO;

    modelVAO->Generate();
	modelVAO->Use();

    usize index_attrib = 0;

    for(const IVariable& var : real_data->m_VariablesSet){
        typename RMI<VBO>::ID vboID;
	    VBO* vertexVBO = ResourcesManager::GetGRM().Create<VBO>(vboID);
	    vertexVBO->Generate(BufferTarget::ARRAY_BUFFER);
        VariableDesc* desc = var.GetVariableDesc();
	    vertexVBO->FillData(var.GetDataPtr(), desc->count * desc->size);
	    modelVAO->BindAttribute<DataType::FLOAT>(index_attrib, *vertexVBO, desc->size, 0, 0);
        index_attrib++;
    }
}

void BackendDispatch::GenerateVAOFromVertexData(const void* data)
{
    const Commands::GenerateVAOFromVertexDataCmd* real_data = reinterpret_cast<const Commands::GenerateVAOFromVertexDataCmd*>(data);
    VAO* modelVAO = real_data->m_VAO;
    const Variable<VertexData, VertexDataDesc>& var = real_data->variable;
    const VertexDataDesc& desc = var.GetConcreteDesc();

    modelVAO->Generate();
	modelVAO->Use();

    typename RMI<VBO>::ID vboID;
	VBO* vertexVBO = ResourcesManager::GetGRM().Create<VBO>(vboID);
	vertexVBO->Generate(BufferTarget::ARRAY_BUFFER);
	vertexVBO->FillData(var.GetDataPtr(), desc.count * desc.size);

    uint32 total = desc.indv_count[0] + desc.indv_count[1] + desc.indv_count[2]; 
    intptr offset = 0;

    for(uint8 i = 0; i < 3; i++){
        uint32 count = desc.indv_count[i];
        modelVAO->BindAttribute<float>(VertexDataDesc::VERTEX + i, *vertexVBO, DataType::FLOAT, count, total, offset);
        offset += count;
    }
}

void BackendDispatch::GenerateIndex(const void* data)
{
    const Commands::GenerateIndexCmd* real_data = reinterpret_cast<const Commands::GenerateIndexCmd*>(data);
    VAO* modelVAO = real_data->m_VAO;
    const Variable<uint32>& var = real_data->m_IndexVariable;
    const VariableDesc& desc = var.GetConcreteDesc();

    typename RMI<VBO>::ID indexVboID;
	VBO* indexVBO = ResourcesManager::GetGRM().Create<VBO>(indexVboID);

	// Set up indices
	indexVBO->Generate(BufferTarget::ELEMENT_ARRAY_BUFFER);
	indexVBO->FillData(var.GetDataPtr(), desc.count * desc.size);
	modelVAO->Unuse();
	indexVBO->Use();
}

void BackendDispatch::CreateTexture(const void* data)
{
    const Commands::CreateTexture* real_data = reinterpret_cast<const Commands::CreateTexture*>(data);
    Texture* tex = real_data->texture;
    const TextureSettings& texture = real_data->settings;

    tex->Generate(texture);
}

TRE_NS_END