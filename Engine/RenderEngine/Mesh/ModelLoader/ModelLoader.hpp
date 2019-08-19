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

	ModelLoader(const Vector<vec3>& vertices, const Vector<uint32>& indices, const Vector<vec2>* textures = NULL, const Vector<vec3>* normals = NULL, const Vector<MatrialForRawModel>& mat_vec = {});
	ModelLoader(const Vector<vec3>& vertices, const Vector<vec2>* textures = NULL, const Vector<vec3>* normals = NULL, const Vector<MatrialForRawModel>& mat_vec = {});

	ModelLoader(Vector<VertexData>& ver_data, Vector<uint32>& indices, const Vector<MatrialForRawModel>& mat_vec);
	ModelLoader(const Vector<VertexData>& ver_data, const Vector<MatrialForRawModel>& mat_vec);

	void ProcessData(StaticMesh& mesh, ShaderID shader_id = 0);

	FORCEINLINE ModelLoader(ModelLoader&& other);

	FORCEINLINE ModelLoader& operator=(ModelLoader&& other);

	FORCEINLINE ModelLoader& operator=(ModelLoader& other) = delete;

	FORCEINLINE ModelLoader(const ModelLoader& other) = delete;


	FORCEINLINE const ssize_t GetVertexCount() const;

	FORCEINLINE const Vector<MatrialForRawModel>& GetMaterials() const;

	FORCEINLINE void Clean();
private:
	// Utility Functions:
	template<ssize_t V, ssize_t T, ssize_t N>
	VAO& LoadFromArray(float(&vert)[V], float(&tex)[T], float(&normal)[N]);
	VAO& LoadFromSettings(const ModelSettings& settings);
	VAO& LoadFromVector(const Vector<vec3>& vertices, const Vector<vec2>* textures, const Vector<vec3>* normals);
	typename Commands::GenerateVAOFromVertexDataCmd* LoadFromVertexData(Vector<VertexData>& ver_data);

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
	VAO& modelVAO = LoadFromArray(vert, tex, normal);
	m_VertexCount = V / 3LLU; // Get the vertex Count!
	modelVAO.Unuse();
	m_IsIndexed = false;

	m_Materials = mat_vec;
	if (m_Materials.IsEmpty()) {
		AbstractMaterial default_material("Unknown");
		m_Materials.EmplaceBack(default_material, m_VertexCount); // default material
	}
}

template<ssize_t V, ssize_t I, ssize_t T, ssize_t N>
ModelLoader::ModelLoader(float(&vert)[V], uint32(&indices)[I], float(&tex)[T], float(&normal)[N], const Vector<MatrialForRawModel>& mat_vec)
{
	VAO& modelVAO = LoadFromArray(vert, tex, normal);
	m_VertexCount = I; // Get the vertex Count!
	m_IsIndexed = true;

	typename RMI<VBO>::ID indexVboID;
	VBO* indexVBO = ResourcesManager::GetGRM().Create<VBO>(indexVboID);
	m_VboIDs.EmplaceBack(indexVboID);

	//Set up indices
	indexVBO->Generate(BufferTarget::ELEMENT_ARRAY_BUFFER);
	indexVBO->FillData(indices);
	modelVAO.Unuse();
	indexVBO->Unuse();

	m_Materials = mat_vec;
	
	if (m_Materials.IsEmpty()) {
		AbstractMaterial default_material("Unknown");
		m_Materials.EmplaceBack(default_material, m_VertexCount); // default material
	}
}

template<ssize_t I>
ModelLoader::ModelLoader(const ModelSettings& settings, uint32(&indices)[I])
{
	ASSERTF((settings.vertexSize == 0 || settings.vertices == NULL), "Attempt to create a ModelLoader with empty vertecies!");
	VAO& modelVAO = LoadFromSettings(settings);
	m_VertexCount = I; // Get the vertex Count!
	m_IsIndexed = true;

	typename RMI<VBO>::ID indexVboID;
	VBO* indexVBO = ResourcesManager::GetGRM().Create<VBO>(indexVboID);
	m_VboIDs.EmplaceBack(indexVboID);

	//Set up indices
	indexVBO->Generate(BufferTarget::ELEMENT_ARRAY_BUFFER);
	indexVBO->FillData(indices);
	modelVAO.Unuse();
	indexVBO->Unuse();
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

FORCEINLINE ModelLoader::~ModelLoader()
{
}

FORCEINLINE void ModelLoader::Clean()
{
}

TRE_NS_END