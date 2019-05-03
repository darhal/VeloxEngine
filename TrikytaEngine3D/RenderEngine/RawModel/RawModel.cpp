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
RawModel<true>::RawModel(const Vector<VertexData>& ver_data, const Vector<uint32>& indices, const Vector<MatrialForRawModel>& mat_vec)
{
	ASSERTF(!(ver_data.size() == 0 || indices.size() == 0), "Attempt to create a RawModel with empty vertecies!");
	LoadFromVertexData(ver_data);
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
RawModel<false>::RawModel(const Vector<VertexData>& ver_data, const Vector<MatrialForRawModel>& mat_vec)
{
	ASSERTF(!(ver_data.size() == 0), "Attempt to create a RawModel with empty vertecies!");
	LoadFromVertexData(ver_data);
	m_VertexCount = ver_data.size() / 8LLU; // Get the vertex Count!
	m_ModelVAO.Unuse();
	m_Materials = mat_vec;
	if (m_Materials.empty()) {
		Material default_material("Unknown");
		m_Materials.emplace_back(default_material, m_VertexCount); // default material
	}
}

template<>
void RawModel<false>::Render(const ShaderProgram& shader) const
{
	int32 lastVertexCount = 0;
	for (const MatrialForRawModel& mat : m_Materials) {
		shader.SetVec3("material.diffuse", mat.material.m_Diffuse);
		shader.SetVec3("material.ambient", mat.material.m_Ambient);
		shader.SetVec3("material.specular", mat.material.m_Specular);
		shader.SetFloat("material.shininess", mat.material.m_Shininess);
		for (const auto& tex : mat.material.m_Textures) {
			glActiveTexture(GL_TEXTURE0 + tex.first);
			tex.second.Use();
		}
		DrawArrays(Primitive::TRIANGLES, lastVertexCount, mat.vcount);
		lastVertexCount += mat.vcount;
	}
}

template<>
void RawModel<true>::Render(const ShaderProgram& shader) const
{
	int32 lastVertexCount = 0;
	for (const MatrialForRawModel& mat : m_Materials) {
		shader.SetVec3("material.diffuse", mat.material.m_Diffuse);
		shader.SetVec3("material.ambient", mat.material.m_Ambient);
		shader.SetVec3("material.specular", mat.material.m_Specular);
		shader.SetFloat("material.shininess", mat.material.m_Shininess);
		for (const auto& tex : mat.material.m_Textures) {
			glActiveTexture(GL_TEXTURE0 + tex.first);
			tex.second.Use();
		}
		DrawElements(Primitive::TRIANGLES, DataType::UINT, mat.vcount, lastVertexCount*sizeof(uint32));
		lastVertexCount += mat.vcount;
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

	//Fill normals:
	normalVBO.Generate(BufferTarget::ARRAY_BUFFER);
	normalVBO.FillData(normal);
	m_ModelVAO.BindAttribute<DataType::FLOAT>(1, normalVBO, 3, 0, 0);
	normalVBO.Unuse();

	//Fill Texture:
	textureVBO.Generate(BufferTarget::ARRAY_BUFFER);
	textureVBO.FillData(tex);
	m_ModelVAO.BindAttribute<DataType::FLOAT>(2, textureVBO, 2, 0, 0);
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

	// Fill normals if availble
	if (normals != NULL && normals->size() != 0) {
		normalVBO.Generate(BufferTarget::ARRAY_BUFFER);
		normalVBO.FillData(&normals->at(0), normals->size() * sizeof(vec3));
		m_ModelVAO.BindAttribute<DataType::FLOAT>(1, normalVBO, 3, 0, 0);
	}

	//Fill Texture if availble
	if (textures != NULL && textures->size() != 0) { 
		textureVBO.Generate(BufferTarget::ARRAY_BUFFER);
		textureVBO.FillData(&textures->at(0), textures->size() * sizeof(vec2));
		m_ModelVAO.BindAttribute<DataType::FLOAT>(2, textureVBO, 2, 0, 0);
	}
}

template<bool B>
void RawModel<B>::LoadFromVertexData(const Vector<VertexData>& ver_data)
{
	ASSERTF(!(ver_data.size() == 0), "Attempt to create a RawModel with empty vertecies!");

	m_ModelVAO.Use();

	//Fill vertex:
	vertexVBO.Generate(BufferTarget::ARRAY_BUFFER);
	vertexVBO.FillData(&ver_data.at(0), ver_data.size() * sizeof(VertexData));
	m_ModelVAO.BindAttribute<float>(0, vertexVBO, DataType::FLOAT, 3, 8, 0);
	m_ModelVAO.BindAttribute<float>(1, vertexVBO, DataType::FLOAT, 3, 8, 3);
	m_ModelVAO.BindAttribute<float>(2, vertexVBO, DataType::FLOAT, 2, 8, 6);
}

TRE_NS_END