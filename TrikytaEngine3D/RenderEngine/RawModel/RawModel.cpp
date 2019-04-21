#include "RawModel.hpp"
#include <Core/Context/Extensions.hpp>
#include <RenderAPI/Shader/ShaderProgram.hpp>

TRE_NS_START

template<>
RawModel<true>::RawModel(const Vector<vec3>& vertices, const Vector<uint32>& indices, const Vector<vec2>* textures, const Vector<vec3>* normals, const Vector<MatrialForRawModel>& mat_vec)
{
	ASSERTF(!(vertices.size() == 0 || indices.size() == 0), "Attempt to create a RawModel with empty vertecies or empty indices!");
	LoadFromVector(vertices, textures, normals);
	m_VertexCount = indices.size(); // Get the vertex Count!
	//Set up indices
	indexVBO.Generate(BufferTarget::ELEMENT_ARRAY_BUFFER);
	indexVBO.FillData(&indices.at(0), indices.size() * sizeof(uint32));
	m_ModelVAO.Unuse();
	indexVBO.Unuse();
	m_Materials = mat_vec;
	if (m_Materials.empty()) {
		m_Materials.emplace_back(Material(), m_VertexCount); // default material
	}
}

template<>
RawModel<false>::RawModel(const Vector<vec3>& vertices, const Vector<vec2>* textures, const Vector<vec3>* normals, const Vector<MatrialForRawModel>& mat_vec)
{
	ASSERTF(!(vertices.size() == 0), "Attempt to create a RawModel with empty vertecies!");
	LoadFromVector(vertices, textures, normals);
	m_VertexCount = vertices.size() / 3LLU; // Get the vertex Count!
	m_ModelVAO.Unuse();
	m_Materials = mat_vec;
	if (m_Materials.empty()) {
		Material default_material("Unknown");
		m_Materials.emplace_back(default_material, m_VertexCount); // default material
	}
}

template<>
RawModel<false>::RawModel(const RawModelSettings& settings)
{
	ASSERTF(!(settings.vertexSize == 0 || settings.vertices == NULL), "Attempt to create a RawModel with empty vertecies!");
	LoadFromSettings(settings);
	m_VertexCount = settings.vertexSize / 3LLU; // Get the vertex Count!
	m_ModelVAO.Unuse();
}

template<>
void RawModel<false>::Render(const ShaderProgram& shader) const
{
	uint32 lastVertexCount = 0;
	for (const MatrialForRawModel& mat : m_Materials) {
		shader.SetVec3("color", mat.material.m_Diffuse);
		DrawArrays(Primitive::TRIANGLES, lastVertexCount, mat.vcount);
		lastVertexCount = mat.vcount;
	}
}

template<>
void RawModel<true>::Render(const ShaderProgram& shader) const
{
	uint32 lastVertexCount = 0;
	for (const MatrialForRawModel& mat : m_Materials) {
		shader.SetVec3("color", mat.material.m_Diffuse);
		DrawElements(Primitive::TRIANGLES, DataType::UINT, mat.vcount, lastVertexCount*sizeof(uint32));
		lastVertexCount = mat.vcount;
	}
}

template<bool B>
template<ssize_t V, ssize_t T, ssize_t N>
void RawModel<B>::LoadFromArray(float(&vert)[V], float(&tex)[T], float(&normal)[N])
{
	m_ModelVAO.Use();

	//Fill vertex:
	vertexVBO.Generate(BufferTarget::ARRAY_BUFFER);
	vertexVBO.FillData(vert);
	m_ModelVAO.BindAttribute<DataType::FLOAT>(0, vertexVBO, 3, 0, 0);

	//Fill Texture:
	textureVBO.Generate(BufferTarget::ARRAY_BUFFER);
	textureVBO.FillData(tex);
	m_ModelVAO.BindAttribute<DataType::FLOAT>(1, textureVBO, 2, 0, 0);

	//Fill normals:
	normalVBO.Generate(BufferTarget::ARRAY_BUFFER);
	normalVBO.FillData(normal);
	m_ModelVAO.BindAttribute<DataType::FLOAT>(2, normalVBO, 3, 0, 0);
	normalVBO.Unuse();
}


template<bool B>
void RawModel<B>::LoadFromVector(const Vector<vec3>& vertices, const Vector<vec2>* textures, const Vector<vec3>* normals)
{
	ASSERTF(!(vertices.size() == 0), "Attempt to create a RawModel with empty vertecies!");

	m_ModelVAO.Use();

	//Fill vertex:
	vertexVBO.Generate(BufferTarget::ARRAY_BUFFER);
	vertexVBO.FillData(&vertices.at(0), vertices.size() * sizeof(vec3));
	m_ModelVAO.BindAttribute<DataType::FLOAT>(0, vertexVBO, 3, 0, 0);
	//Fill Texture if availble
	if (textures != NULL && textures->size() != 0) { 
		textureVBO.Generate(BufferTarget::ARRAY_BUFFER);
		textureVBO.FillData(&textures->at(0), textures->size() * sizeof(vec2));
		m_ModelVAO.BindAttribute<DataType::FLOAT>(1, textureVBO, 2, 0, 0);
	}

	// Fill normals if availble
	if (normals != NULL && normals->size() != 0) { 
		normalVBO.Generate(BufferTarget::ARRAY_BUFFER);
		normalVBO.FillData(&normals->at(0), normals->size() * sizeof(vec3));
		m_ModelVAO.BindAttribute<DataType::FLOAT>(2, normalVBO, 3, 0, 0);
	}
}

TRE_NS_END