#pragma once

#include "ModelSettings.hpp"

#include <Core/Misc/Defines/Common.hpp>
#include <Core/Misc/Defines/DataStructure.hpp>

#include <RenderAPI/VertexArray/VAO.hpp>
#include <RenderAPI/VertexBuffer/VBO.hpp>
#include <RenderAPI/General/GLContext.hpp>
#include <RenderAPI/Shader/ShaderProgram.hpp>

#include <RenderEngine/Materials/Material.hpp>
#include <RenderEngine/Mesh/StaticMesh/StaticMesh.hpp>
#include "RenderEngine/Renderer/Backend/Commands/Commands.hpp"
#include <RenderEngine/Managers/ResourcesManager/ResourcesManager.hpp>
#include <RenderEngine/Materials/AbstractMaterial/AbstractMaterial.hpp>

TRE_NS_START

struct MatrialForRawModel
{
	const AbstractMaterial& material;
	int32 vcount;
	MatrialForRawModel(const AbstractMaterial& material, int32 vcount) : material(material), vcount(vcount)
	{}

	MatrialForRawModel(const MatrialForRawModel& other) : material(other.material), vcount(other.vcount)
	{}
};


class ModelLoader
{
public:
	ModelLoader() = delete; // no default constructor sorry :(

	FORCEINLINE ~ModelLoader();

	template<ssize_t V, ssize_t T, ssize_t N>
	ModelLoader(float(&vert)[V], float(&tex)[T], float(&normal)[N], const Vector<MatrialForRawModel>& mat_vec = {});

	template<ssize_t V, ssize_t I, ssize_t T, ssize_t N>
	ModelLoader(float(&vert)[V], uint32(&indices)[I], float(&tex)[T], float(&normal)[N], const Vector<MatrialForRawModel>& mat_vec = {});

	ModelLoader(const ModelSettings& settings);

	template<ssize_t I>
	ModelLoader(const ModelSettings&, uint32(&indices)[I]);

	ModelLoader(Vector<vec3>& vertices, Vector<uint32>& indices, Vector<vec2>* textures = NULL, Vector<vec3>* normals = NULL, const Vector<MatrialForRawModel>& mat_vec = {});
	ModelLoader(Vector<vec3>& vertices, Vector<vec2>* textures = NULL, Vector<vec3>* normals = NULL, const Vector<MatrialForRawModel>& mat_vec = {});

	ModelLoader(Vector<VertexData>& ver_data, Vector<uint32>& indices, const Vector<MatrialForRawModel>& mat_vec);
	ModelLoader(Vector<VertexData>& ver_data, const Vector<MatrialForRawModel>& mat_vec);

	void ProcessData(StaticMesh& mesh, ShaderID shader_id = 0);

	FORCEINLINE ModelLoader(ModelLoader&& other);

	FORCEINLINE ModelLoader& operator=(ModelLoader&& other);

	FORCEINLINE ModelLoader& operator=(ModelLoader& other) = delete;

	FORCEINLINE ModelLoader(const ModelLoader& other) = delete;


	FORCEINLINE const ssize_t GetVertexCount() const;

	FORCEINLINE const Vector<MatrialForRawModel>& GetMaterials() const;

	FORCEINLINE Vector<MatrialForRawModel>& GetMaterials();

	FORCEINLINE void Clean();
private:
	// Utility Functions:
	template<ssize_t V, ssize_t T, ssize_t N>
	Commands::CreateVAO* LoadFromArray(float(&vert)[V], float(&tex)[T], float(&normal)[N]);
	Commands::CreateVAO* LoadFromSettings(const ModelSettings& settings);
	Commands::CreateVAO* LoadFromVector(Vector<vec3>& vertices, Vector<vec2>* textures, Vector<vec3>* normals);
	typename Commands::CreateVAO* LoadFromVertexData(Vector<VertexData>& ver_data);

	Scene* m_Scene;

	ssize_t m_VertexCount;
	typename RMI<VAO>::ID m_VaoID;
	Vector<typename RMI<VBO>::ID> m_VboIDs;	
	Vector<MatrialForRawModel> m_Materials;
	bool m_IsIndexed;
};

template<ssize_t V, ssize_t T, ssize_t N>
ModelLoader::ModelLoader(float(&vert)[V], float(&tex)[T], float(&normal)[N], const Vector<MatrialForRawModel>& mat_vec)
{
	m_VertexCount = V / 3LLU; // Get the vertex Count!
	m_IsIndexed = false;

	Commands::CreateVAO* create_vao_cmd = LoadFromArray(vert, tex, normal);
	// modelVAO.Unuse();

	m_Materials = mat_vec;
	if (m_Materials.IsEmpty()) {
		m_Materials.EmplaceBack(*(new AbstractMaterial()), m_VertexCount); // default material
	}
}

template<ssize_t V, ssize_t I, ssize_t T, ssize_t N>
ModelLoader::ModelLoader(float(&vert)[V], uint32(&indices)[I], float(&tex)[T], float(&normal)[N], const Vector<MatrialForRawModel>& mat_vec)
{
	m_VertexCount = I; // Get the vertex Count!
	m_IsIndexed = true;

	Commands::CreateVAO* create_vao_cmd = LoadFromArray(vert, tex, normal);
	
	// Set up indices
	typename RMI<VBO>::ID indexVboID;
	VBO* indexVBO = ResourcesManager::GetGRM().Create<VBO>(indexVboID);
	m_VboIDs.EmplaceBack(indexVboID);

	IRenderer::ResourcesCmdBuffer& CmdBucket = RenderManager::GetRenderer().GetResourcesCommandBuffer();
	Commands::CreateIndexBuffer* create_index_cmd
		= CmdBucket.AppendCommand<Commands::CreateIndexBuffer>(create_vao_cmd);
	create_index_cmd->vao = create_vao_cmd->vao;
	create_index_cmd->settings = VertexSettings::VertexBufferData(indices, I, indexVBO);

	m_Materials = mat_vec;
	if (m_Materials.IsEmpty()) {
		m_Materials.EmplaceBack(*(new AbstractMaterial()), m_VertexCount); // default material
	}
}

template<ssize_t I>
ModelLoader::ModelLoader(const ModelSettings& settings, uint32(&indices)[I])
{
	ASSERTF((settings.vertexSize == 0 || settings.vertices == NULL), "Attempt to create a ModelLoader with empty vertecies!");
	m_VertexCount = I; // Get the vertex Count!
	m_IsIndexed = true;

	Commands::CreateVAO* create_vao_cmd = LoadFromSettings(settings);

	// Set up indices
	typename RMI<VBO>::ID indexVboID;
	VBO* indexVBO = ResourcesManager::GetGRM().Create<VBO>(indexVboID);
	m_VboIDs.EmplaceBack(indexVboID);

	IRenderer::ResourcesCmdBuffer& CmdBucket = RenderManager::GetRenderer().GetResourcesCommandBuffer();
	Commands::CreateIndexBuffer* create_index_cmd
		= CmdBucket.AppendCommand<Commands::CreateIndexBuffer>(create_vao_cmd);
	create_index_cmd->vao = create_vao_cmd->vao;
	create_index_cmd->settings = VertexSettings::VertexBufferData(indices, I, indexVBO);

	m_Materials.EmplaceBack(*(new AbstractMaterial()), m_VertexCount); // default material
}

FORCEINLINE ModelLoader::ModelLoader(ModelLoader&& other) :
	m_VertexCount(other.m_VertexCount), m_VaoID(other.m_VaoID),
	m_VboIDs(std::move(other.m_VboIDs)), m_Materials(std::move(other.m_Materials)), m_IsIndexed(other.m_IsIndexed)
{
}

FORCEINLINE ModelLoader& ModelLoader::operator=(ModelLoader&& other) {
	m_VertexCount = other.m_VertexCount;
	m_VaoID = other.m_VaoID;
	m_VboIDs = std::move(other.m_VboIDs); 
	m_Materials = std::move(other.m_Materials); 
	m_IsIndexed = other.m_IsIndexed;
	return *this;
}

FORCEINLINE const ssize_t ModelLoader::GetVertexCount() const
{
	return m_VertexCount;
}

FORCEINLINE const Vector<MatrialForRawModel>& ModelLoader::GetMaterials() const
{
	return m_Materials;
}

FORCEINLINE Vector<MatrialForRawModel>& ModelLoader::GetMaterials() 
{
	return m_Materials;
}

FORCEINLINE ModelLoader::~ModelLoader()
{
}

FORCEINLINE void ModelLoader::Clean()
{
}

TRE_NS_END