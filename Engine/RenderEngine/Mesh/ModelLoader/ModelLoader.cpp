#include "ModelLoader.hpp"
#include <Core/Context/Extensions.hpp>
#include <RenderAPI/Shader/ShaderProgram.hpp>
#include "RenderEngine/Managers/RenderManager/RenderManager.hpp"

TRE_NS_START

ModelLoader::ModelLoader(const Vector<vec3>& vertices, const Vector<uint32>& indices, const Vector<vec2>* textures, const Vector<vec3>* normals, const Vector<MatrialForRawModel>& mat_vec)
{
	ASSERTF((vertices.Size() == 0 || indices.Size() == 0), "Attempt to create a ModelLoader with empty vertecies or empty indices!");
	
	VAO& modelVAO = LoadFromVector(vertices, textures, normals);
	m_VertexCount = indices.Size(); // Get the vertex Count!
	m_IsIndexed = true;

	typename RMI<VBO>::ID indexVboID;
	VBO* indexVBO = ResourcesManager::GetGRM().Create<VBO>(indexVboID);
	m_VboIDs.EmplaceBack(indexVboID);

	//Set up indices
	indexVBO->Generate(BufferTarget::ELEMENT_ARRAY_BUFFER);
	indexVBO->FillData(&indices.At(0), indices.Size() * sizeof(uint32));
	modelVAO.Unbind();
	indexVBO->Unbind();

	m_Materials = mat_vec;
	if (m_Materials.IsEmpty()) {
		m_Materials.EmplaceBack(AbstractMaterial(), m_VertexCount); // default material
	}
}

ModelLoader::ModelLoader(const Vector<vec3>& vertices, const Vector<vec2>* textures, const Vector<vec3>* normals, const Vector<MatrialForRawModel>& mat_vec)
{
	ASSERTF((vertices.Size() == 0), "Attempt to create a ModelLoader with empty vertecies!");
	
	VAO& modelVAO = LoadFromVector(vertices, textures, normals);
	m_VertexCount = vertices.Size() / 3LLU; // Get the vertex Count!
	m_IsIndexed = false;

	modelVAO.Unuse();
	m_Materials = mat_vec;
	
	if (m_Materials.IsEmpty()) {
		AbstractMaterial default_material("Unknown");
		m_Materials.EmplaceBack(default_material, m_VertexCount); // default material
	}
}

ModelLoader::ModelLoader(const ModelSettings& settings)
{
	ASSERTF((settings.vertexSize == 0 || settings.vertices == NULL), "Attempt to create a ModelLoader with empty vertecies!");
	VAO& modelVAO = LoadFromSettings(settings);
	m_VertexCount = settings.vertexSize / 3LLU; // Get the vertex Count!
	modelVAO.Unuse();
}

ModelLoader::ModelLoader(Vector<VertexData>& ver_data, Vector<uint32>& indices, const Vector<MatrialForRawModel>& mat_vec)
{
	ASSERTF((ver_data.Size() == 0 || indices.Size() == 0), "Attempt to create a ModelLoader with empty vertecies!");
	
	m_VertexCount = indices.Size(); // Get the vertex Count!
	m_IsIndexed = true;

	typename Commands::GenerateVAOFromVertexDataCmd* create_vao_cmd = LoadFromVertexData(ver_data);
	
	IRenderer::ResourcesCmdBuffer& CmdBucket = RenderManager::GetRenderer().GetResourcesCommandBuffer();	

	Commands::GenerateIndexCmd* create_index_cmd 
		= CmdBucket.AppendCommand<Commands::GenerateIndexCmd>(create_vao_cmd);
	
	VariableDesc desc;
	desc.count = (uint32) indices.Size();
	Variable<uint32> var_index(desc, indices.StealPtr());
	// memcpy(var_index.GetPointer(), indices.Front(), desc.count * desc.size);

	create_index_cmd->m_VAO = create_vao_cmd->m_VAO;
	create_index_cmd->m_IndexVariable = var_index;

	/*typename RMI<VBO>::ID indexVboID;
	VBO* indexVBO = ResourcesManager::GetGRM().Create<VBO>(indexVboID);
	m_VboIDs.EmplaceBack(indexVboID);

	//Set up indices
	indexVBO->Generate(BufferTarget::ELEMENT_ARRAY_BUFFER);
	indexVBO->FillData(&indices.At(0), indices.Size() * sizeof(uint32));
	((VAO*)create_vao_cmd)->Unbind();
	indexVBO->Unbind();*/

	m_Materials = mat_vec;

	if (m_Materials.IsEmpty()) {
		m_Materials.EmplaceBack(AbstractMaterial(), m_VertexCount); // default material
	}
}

ModelLoader::ModelLoader(const Vector<VertexData>& ver_data, const Vector<MatrialForRawModel>& mat_vec)
{
	ASSERTF((ver_data.Size() == 0), "Attempt to create a ModelLoader with empty vertecies!");
	// VAO& modelVAO = LoadFromVertexData(ver_data);
	m_VertexCount = ver_data.Size() / 8LLU; // Get the vertex Count!
	m_IsIndexed = false;

	// modelVAO.Unuse();
	m_Materials = mat_vec;

	if (m_Materials.IsEmpty()) {
		AbstractMaterial default_material("Unknown");
		m_Materials.EmplaceBack(default_material, m_VertexCount); // default material
	}
}

template<ssize_t V, ssize_t T, ssize_t N>
VAO& ModelLoader::LoadFromArray(float(&vert)[V], float(&tex)[T], float(&normal)[N])
{
	VAO* modelVAO = ResourcesManager::GetGRM().Create<VAO>(m_VaoID);
	modelVAO->Generate();
	modelVAO->Use();
	
	typename RMI<VBO>::ID vboID;

	//Fill vertex:
	VBO* vertexVBO = ResourcesManager::GetGRM().Create<VBO>(vboID);
	m_VboIDs.EmplaceBack(vboID);
	vertexVBO->Generate(BufferTarget::ARRAY_BUFFER);
	vertexVBO->FillData(vert);
	modelVAO->BindAttribute<DataType::FLOAT>(0, *vertexVBO, 3, 0, 0);

	//Fill normals:
	VBO* normalVBO = ResourcesManager::GetGRM().Create<VBO>(vboID);
	m_VboIDs.EmplaceBack(vboID);
	normalVBO->Generate(BufferTarget::ARRAY_BUFFER);
	normalVBO->FillData(normal);
	modelVAO->BindAttribute<DataType::FLOAT>(1, *normalVBO, 3, 0, 0);
	normalVBO->Unuse();

	//Fill Texture:
	VBO* textureVBO = ResourcesManager::GetGRM().Create<VBO>(vboID);
	m_VboIDs.EmplaceBack(vboID);
	textureVBO->Generate(BufferTarget::ARRAY_BUFFER);
	textureVBO->FillData(tex);
	modelVAO->BindAttribute<DataType::FLOAT>(2, *textureVBO, 2, 0, 0);

	return *modelVAO;
}

VAO& ModelLoader::LoadFromVector(const Vector<vec3>& vertices, const Vector<vec2>* textures, const Vector<vec3>* normals)
{
	ASSERTF((vertices.Size() == 0), "Attempt to create a ModelLoader with empty vertecies!");
	
	VAO* modelVAO = ResourcesManager::GetGRM().Create<VAO>(m_VaoID);
	modelVAO->Generate();
	modelVAO->Use();

	//Fill vertex:
	typename RMI<VBO>::ID vboID;
	VBO* vertexVBO = ResourcesManager::GetGRM().Create<VBO>(vboID);
	m_VboIDs.EmplaceBack(vboID);

	vertexVBO->Generate(BufferTarget::ARRAY_BUFFER);
	vertexVBO->FillData(&vertices.At(0), vertices.Size() * sizeof(vec3));
	modelVAO->BindAttribute<DataType::FLOAT>(0, *vertexVBO, 3, 0, 0);

	// Fill normals if availble
	if (normals != NULL && normals->Size() != 0) {
		VBO* normalVBO = ResourcesManager::GetGRM().Create<VBO>(vboID);
		m_VboIDs.EmplaceBack(vboID);

		normalVBO->Generate(BufferTarget::ARRAY_BUFFER);
		normalVBO->FillData(&normals->At(0), normals->Size() * sizeof(vec3));
		modelVAO->BindAttribute<DataType::FLOAT>(1, *normalVBO, 3, 0, 0);
	}

	// Fill Texture if availble
	if (textures != NULL && textures->Size() != 0) {
		VBO* textureVBO = ResourcesManager::GetGRM().Create<VBO>(vboID);
		m_VboIDs.EmplaceBack(vboID);

		textureVBO->Generate(BufferTarget::ARRAY_BUFFER);
		textureVBO->FillData(&textures->At(0), textures->Size() * sizeof(vec2));
		modelVAO->BindAttribute<DataType::FLOAT>(2, *textureVBO, 2, 0, 0);
	}

	return *modelVAO;
}

typename Commands::GenerateVAOFromVertexDataCmd* ModelLoader::LoadFromVertexData(Vector<VertexData>& ver_data)
{
	ASSERTF((ver_data.Size() == 0), "Attempt to create a ModelLoader with empty vertecies!");
	VAO* modelVAO = ResourcesManager::GetGRM().Create<VAO>(m_VaoID);

	/*
	printf("->->-> VAO ID = %d\n", m_VaoID);
	modelVAO->Generate();
	modelVAO->Bind();

	typename RMI<VBO>::ID vboID;
	VBO* vertexVBO = ResourcesManager::GetGRM().Create<VBO>(vboID);
	m_VboIDs.EmplaceBack(vboID);

	//Fill vertex:
	vertexVBO->Generate(BufferTarget::ARRAY_BUFFER);
	vertexVBO->FillData(&ver_data.At(0), ver_data.Size() * sizeof(VertexData));
	modelVAO->BindAttribute<float>(0, *vertexVBO, DataType::FLOAT, 3, 8, 0);
	modelVAO->BindAttribute<float>(1, *vertexVBO, DataType::FLOAT, 3, 8, 3);
	modelVAO->BindAttribute<float>(2, *vertexVBO, DataType::FLOAT, 2, 8, 6);
	vertexVBO->Unbind();*/

	IRenderer::ResourcesCmdBuffer& CmdBucket = RenderManager::GetRenderer().GetResourcesCommandBuffer();
	Commands::GenerateVAOFromVertexDataCmd* create_vao_cmd 
		= CmdBucket.template AddCommand<Commands::GenerateVAOFromVertexDataCmd>(typename IRenderer::ResourcesCmdBuffer::Key(-1));
	
	VertexDataDesc desc;
	desc.count = (uint32) ver_data.Size();
	desc.freq = VariableDesc::FREQ_VERTEX;
	desc.indv_count[0] = 3;
	desc.indv_count[1] = 3;
	desc.indv_count[2] = 2;

	Variable<VertexData, VertexDataDesc> var(desc, ver_data.StealPtr());
	//memcpy(var.GetPointer(), ver_data.Front(), desc.count * desc.size);
	create_vao_cmd->variable = var;
	create_vao_cmd->m_VAO = modelVAO;
	
	return create_vao_cmd;
}

VAO& ModelLoader::LoadFromSettings(const ModelSettings& settings)
{
	ASSERTF((settings.vertexSize == 0 || settings.vertices == NULL), "Attempt to create a ModelLoader with empty vertecies!");
	
	VAO* modelVAO = ResourcesManager::GetGRM().Create<VAO>(m_VaoID);
	modelVAO->Generate();
	modelVAO->Use();

	//Fill vertex:
	typename RMI<VBO>::ID vboID;
	VBO* vertexVBO = ResourcesManager::GetGRM().Create<VBO>(vboID);
	m_VboIDs.EmplaceBack(vboID);

	vertexVBO->Generate(BufferTarget::ARRAY_BUFFER);
	vertexVBO->FillData(settings.vertices, settings.vertexSize * sizeof(float));
	modelVAO->BindAttribute<DataType::FLOAT>(0, *vertexVBO, 3, 0, 0);

	if (settings.normalSize != 0 && settings.normals != NULL) { // Fill normals if availble
		VBO* normalVBO = ResourcesManager::GetGRM().Create<VBO>(vboID);
		m_VboIDs.EmplaceBack(vboID);

		//Fill normals:
		normalVBO->Generate(BufferTarget::ARRAY_BUFFER);
		normalVBO->FillData(settings.normals, settings.normalSize * sizeof(float));
		modelVAO->BindAttribute<DataType::FLOAT>(1, *normalVBO, 3, 0, 0);
		normalVBO->Unuse();
	}

	if (settings.textureSize != 0 && settings.textures != NULL) { //Fill Texture if availble
		VBO* textureVBO = ResourcesManager::GetGRM().Create<VBO>(vboID);
		m_VboIDs.EmplaceBack(vboID);

		textureVBO->Generate(BufferTarget::ARRAY_BUFFER);
		textureVBO->FillData(settings.textures, settings.textureSize * sizeof(float));
		modelVAO->BindAttribute<DataType::FLOAT>(2, *textureVBO, 2, 0, 0);
	}

	return *modelVAO;
}

void ModelLoader::ProcessData(StaticMesh& mesh, ShaderID shader_id)
{
	mesh.SetVaoID(m_VaoID);
	int32 lastVertexCount = 0;

	if (m_IsIndexed){ // Not = to 0 means that its indexed.
		for (const MatrialForRawModel& mat : m_Materials) {
			PrimitiveGeometry model_geo(DataType::UINT, mat.vcount, lastVertexCount * sizeof(uint32));
			MaterialID matID = ResourcesManager::GetGRM().Generate<Material>(mat.material, shader_id);
			mesh.AddSubMesh(model_geo, matID);
			lastVertexCount += mat.vcount;
		}
	}else{
		for (const MatrialForRawModel& mat : m_Materials) {
			PrimitiveGeometry model_geo(lastVertexCount, mat.vcount);
			MaterialID matID = ResourcesManager::GetGRM().Generate<Material>(mat.material, shader_id);
			mesh.AddSubMesh(model_geo, matID);
			lastVertexCount += mat.vcount;
		}
	}
}

TRE_NS_END