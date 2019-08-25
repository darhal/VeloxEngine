#include "ModelLoader.hpp"
#include <Core/Context/Extensions.hpp>
#include <RenderAPI/Shader/ShaderProgram.hpp>
#include "RenderEngine/Managers/RenderManager/RenderManager.hpp"

TRE_NS_START

ModelLoader::ModelLoader(Vector<vec3>& vertices, Vector<uint32>& indices, Vector<vec2>* textures, Vector<vec3>* normals, const Vector<MatrialForRawModel>& mat_vec)
{
	ASSERTF((vertices.Size() == 0 || indices.Size() == 0), "Attempt to create a ModelLoader with empty vertecies or empty indices!");
	
	m_VertexCount = indices.Size(); // Get the vertex Count!
	m_IsIndexed = true;

	Commands::CreateVAO* create_vao_cmd = LoadFromVector(vertices, textures, normals);

	// Set up indices
	typename RMI<VBO>::ID indexVboID;
	VBO* indexVBO = ResourcesManager::GetGRM().Create<VBO>(indexVboID);
	m_VboIDs.EmplaceBack(indexVboID);

	IRenderer::ResourcesCmdBuffer& CmdBucket = RenderManager::GetRenderer().GetResourcesCommandBuffer();
	Commands::CreateIndexBuffer* create_index_cmd
		= CmdBucket.AppendCommand<Commands::CreateIndexBuffer>(create_vao_cmd);
	create_index_cmd->vao = create_vao_cmd->vao;
	create_index_cmd->settings = VertexSettings::VertexBufferData(indices, indexVBO);

	m_Materials = mat_vec;
	if (m_Materials.IsEmpty()) {
		m_Materials.EmplaceBack(*(new AbstractMaterial()), m_VertexCount); // default material
	}
}

ModelLoader::ModelLoader(Vector<vec3>& vertices, Vector<vec2>* textures, Vector<vec3>* normals, const Vector<MatrialForRawModel>& mat_vec)
{
	ASSERTF((vertices.Size() == 0), "Attempt to create a ModelLoader with empty vertecies!");
	
	m_VertexCount = vertices.Size() / 3LLU; // Get the vertex Count!
	m_IsIndexed = false;

	Commands::CreateVAO* create_vao_cmd = LoadFromVector(vertices, textures, normals);
	// modelVAO.Unuse(); // maybe append another command to order it to unuse the vao.

	m_Materials = mat_vec;
	if (m_Materials.IsEmpty()) {
		m_Materials.EmplaceBack(*(new AbstractMaterial()), m_VertexCount); // default material
	}
}

ModelLoader::ModelLoader(const ModelSettings& settings)
{
	ASSERTF((settings.vertexSize == 0 || settings.vertices == NULL), "Attempt to create a ModelLoader with empty vertecies!");

	m_VertexCount = settings.vertexSize / 3LLU; // Get the vertex Count!

	Commands::CreateVAO* create_vao_cmd = LoadFromSettings(settings);
	// modelVAO.Unuse(); // maybe append another command to order it to unuse the vao.

	m_Materials.EmplaceBack(*(new AbstractMaterial()), m_VertexCount); // default material
}

ModelLoader::ModelLoader(Vector<VertexData>& ver_data, Vector<uint32>& indices, const Vector<MatrialForRawModel>& mat_vec)
{
	ASSERTF((ver_data.Size() == 0 || indices.Size() == 0), "Attempt to create a ModelLoader with empty vertecies!");
	
	m_VertexCount = indices.Size(); // Get the vertex Count!
	m_IsIndexed = true;

	Commands::CreateVAO* create_vao_cmd = LoadFromVertexData(ver_data);
	
	VboID vboID;
	VBO* indexVBO = ResourcesManager::GetGRM().Create<VBO>(vboID);
	m_VboIDs.EmplaceBack(vboID);

	IRenderer::ResourcesCmdBuffer& CmdBucket = RenderManager::GetRenderer().GetResourcesCommandBuffer();	
	Commands::CreateIndexBuffer* create_index_cmd
		= CmdBucket.AppendCommand<Commands::CreateIndexBuffer>(create_vao_cmd);
	create_index_cmd->vao = create_vao_cmd->vao;
	create_index_cmd->settings = VertexSettings::VertexBufferData(indices, indexVBO);

	m_Materials = mat_vec;

	if (m_Materials.IsEmpty()) {
		m_Materials.EmplaceBack(*(new AbstractMaterial()), m_VertexCount); // default material
	}
}

ModelLoader::ModelLoader(Vector<VertexData>& ver_data, const Vector<MatrialForRawModel>& mat_vec)
{
	ASSERTF((ver_data.Size() == 0), "Attempt to create a ModelLoader with empty vertecies!");
	m_VertexCount = ver_data.Size() / 8LLU; // Get the vertex Count!
	m_IsIndexed = false;

	Commands::CreateVAO* create_vao_cmd = LoadFromVertexData(ver_data);
	// modelVAO.Unuse(); // maybe append another command to order it to unuse the vao.

	m_Materials = mat_vec;
	if (m_Materials.IsEmpty()) {
		m_Materials.EmplaceBack(*(new AbstractMaterial()), m_VertexCount); // default material
	}
}

template<ssize_t V, ssize_t T, ssize_t N>
Commands::CreateVAO* ModelLoader::LoadFromArray(float(&vert)[V], float(&tex)[T], float(&normal)[N])
{
	VAO* modelVAO = ResourcesManager::GetGRM().Create<VAO>(m_VaoID);

	IRenderer::ResourcesCmdBuffer& CmdBucket = RenderManager::GetRenderer().GetResourcesCommandBuffer();
	Commands::CreateVAO* create_vao_cmd
		= CmdBucket.template AddCommand<Commands::CreateVAO>(typename IRenderer::ResourcesCmdBuffer::Key(-1));
	create_vao_cmd->vao = modelVAO;
	create_vao_cmd->settings = VertexSettings();

	typename RMI<VBO>::ID vboID;

	// Fill vertex positions:
	VBO* vertexVBO = ResourcesManager::GetGRM().Create<VBO>(vboID);
	m_VboIDs.EmplaceBack(vboID);
	create_vao_cmd->settings.vertices_data.EmplaceBack(vert, V, vertexVBO);

	// Fill normals:
	VBO* normalVBO = ResourcesManager::GetGRM().Create<VBO>(vboID);
	m_VboIDs.EmplaceBack(vboID);
	create_vao_cmd->settings.vertices_data.EmplaceBack(normal, N, normalVBO);

	// Fill Textures:
	VBO* textureVBO = ResourcesManager::GetGRM().Create<VBO>(vboID);
	m_VboIDs.EmplaceBack(vboID);
	create_vao_cmd->settings.vertices_data.EmplaceBack(tex, T, textureVBO);

	uint32 attrib_data_sizes[] = { 3, 3, 2 };
	uint8 attrib_index = 0;

	for (const uint32 size : attrib_data_sizes) {
		create_vao_cmd->settings.attributes.EmplaceBack(attrib_index, size, 0, 0);
		attrib_index++;
	}

	return create_vao_cmd;
}

Commands::CreateVAO* ModelLoader::LoadFromVector(Vector<vec3>& vertices, Vector<vec2>* textures, Vector<vec3>* normals)
{
	ASSERTF((vertices.Size() == 0), "Attempt to create a ModelLoader with empty vertecies!");
	
	VAO* modelVAO = ResourcesManager::GetGRM().Create<VAO>(m_VaoID);

	IRenderer::ResourcesCmdBuffer& CmdBucket = RenderManager::GetRenderer().GetResourcesCommandBuffer();
	Commands::CreateVAO* create_vao_cmd
		= CmdBucket.template AddCommand<Commands::CreateVAO>(typename IRenderer::ResourcesCmdBuffer::Key(-1));
	create_vao_cmd->vao = modelVAO;
	create_vao_cmd->settings = VertexSettings();

	//Fill vertex:
	typename RMI<VBO>::ID vboID;
	VBO* vertexVBO = ResourcesManager::GetGRM().Create<VBO>(vboID);
	m_VboIDs.EmplaceBack(vboID);
	create_vao_cmd->settings.vertices_data.EmplaceBack(vertices, vertexVBO);
	create_vao_cmd->settings.attributes.EmplaceBack(0, 3, 0, 0);

	// Fill normals if availble
	if (normals != NULL && normals->Size() != 0) {
		VBO* normalVBO = ResourcesManager::GetGRM().Create<VBO>(vboID);
		m_VboIDs.EmplaceBack(vboID);

		create_vao_cmd->settings.vertices_data.EmplaceBack(*normals, normalVBO);
		create_vao_cmd->settings.attributes.EmplaceBack(1, 3, 0, 0);
	}

	// Fill Texture if availble
	if (textures != NULL && textures->Size() != 0) {
		VBO* textureVBO = ResourcesManager::GetGRM().Create<VBO>(vboID);
		m_VboIDs.EmplaceBack(vboID);

		create_vao_cmd->settings.vertices_data.EmplaceBack(*textures, textureVBO);
		create_vao_cmd->settings.attributes.EmplaceBack(2, 2, 0, 0);
	}

	return create_vao_cmd;
}

Commands::CreateVAO* ModelLoader::LoadFromVertexData(Vector<VertexData>& ver_data)
{
	ASSERTF((ver_data.Size() == 0), "Attempt to create a ModelLoader with empty vertecies!");
	VAO* modelVAO = ResourcesManager::GetGRM().Create<VAO>(m_VaoID);
	VboID vboID;
	VBO* vbo = ResourcesManager::GetGRM().Create<VBO>(vboID);
	m_VboIDs.EmplaceBack(vboID);

	IRenderer::ResourcesCmdBuffer& CmdBucket = RenderManager::GetRenderer().GetResourcesCommandBuffer();
	Commands::CreateVAO* create_vao_cmd
		= CmdBucket.template AddCommand<Commands::CreateVAO>(typename IRenderer::ResourcesCmdBuffer::Key(-1));
	create_vao_cmd->vao = modelVAO;
	create_vao_cmd->settings = VertexSettings();

	create_vao_cmd->settings.vertices_data.EmplaceBack(ver_data, vbo);

	uint32 attrib_data_sizes[] = { 3, 3, 2 };
	uint32 offset = 0;
	uint8 attrib_index = 0;

	for (const uint32 size : attrib_data_sizes) {
		create_vao_cmd->settings.attributes.EmplaceBack(attrib_index, size, 8, offset);
		attrib_index++;
		offset += size;
	}

	return create_vao_cmd;
}

Commands::CreateVAO* ModelLoader::LoadFromSettings(const ModelSettings& settings)
{
	ASSERTF((settings.vertexSize == 0 || settings.vertices == NULL), "Attempt to create a ModelLoader with empty vertecies!");

	typename RMI<VBO>::ID vboID;
	VAO* modelVAO = ResourcesManager::GetGRM().Create<VAO>(m_VaoID);

	IRenderer::ResourcesCmdBuffer& CmdBucket = RenderManager::GetRenderer().GetResourcesCommandBuffer();
	Commands::CreateVAO* create_vao_cmd
		= CmdBucket.template AddCommand<Commands::CreateVAO>(typename IRenderer::ResourcesCmdBuffer::Key(-1));
	create_vao_cmd->vao = modelVAO;
	create_vao_cmd->settings = VertexSettings();

	// Fill vertex:
	VBO* vertexVBO = ResourcesManager::GetGRM().Create<VBO>(vboID);
	m_VboIDs.EmplaceBack(vboID);
	create_vao_cmd->settings.vertices_data.EmplaceBack(settings.vertices, settings.vertexSize, vertexVBO);
	create_vao_cmd->settings.attributes.EmplaceBack(0, 3, 0, 0);

	if (settings.normalSize != 0 && settings.normals != NULL) { // Fill normals if availble
		VBO* normalVBO = ResourcesManager::GetGRM().Create<VBO>(vboID);
		m_VboIDs.EmplaceBack(vboID);

		// Fill normals:
		create_vao_cmd->settings.vertices_data.EmplaceBack(settings.normals, settings.normalSize, normalVBO);
		create_vao_cmd->settings.attributes.EmplaceBack(1, 3, 0, 0);
	}

	if (settings.textureSize != 0 && settings.textures != NULL) { // Fill Texture if availble
		VBO* textureVBO = ResourcesManager::GetGRM().Create<VBO>(vboID);
		m_VboIDs.EmplaceBack(vboID);

		// Fill textures:
		create_vao_cmd->settings.vertices_data.EmplaceBack(settings.textures, settings.textureSize, textureVBO);
		create_vao_cmd->settings.attributes.EmplaceBack(2, 2, 0, 0);
	}

	return create_vao_cmd;
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