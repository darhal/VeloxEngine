#pragma once

#include <Core/Misc/Defines/Common.hpp>
#include <RenderAPI/VertexArray/VAO.hpp>
#include <RenderAPI/VertexBuffer/VBO.hpp>
#include <RenderAPI/General/GLContext.hpp>
#include "RawModelSettings.hpp"
#include <Core/Misc/Defines/DataStructure.hpp>

TRE_NS_START

template<bool IsIndexed>
class RawModel
{
public:
	RawModel() = delete; // no default constructor sorry :(

	~RawModel();

	template<ssize_t V, ssize_t T, ssize_t N>
	RawModel(float(&vert)[V], float(&tex)[T], float(&normal)[N]);

	RawModel(const RawModelSettings& settings);


	template<ssize_t V, ssize_t T, ssize_t N, ssize_t I>
	RawModel(float(&vert)[V], float(&tex)[T], float(&normal)[N], uint32(&indices)[I]);

	template<ssize_t I>
	RawModel(const RawModelSettings&, uint32(&indices)[I]);

	RawModel(const Vector<vec3>& vertices, const Vector<vec2>* textures, const Vector<vec3>* normals, const Vector<uint32>& indices);
	RawModel(const Vector<vec3>& vertices, const Vector<vec2>* textures = NULL, const Vector<vec3>* normals = NULL);

	void Render() const;

	FORCEINLINE void Use(const ShaderProgram& shader) const;
	FORCEINLINE const VAO& GetVAO() const { return m_ModelVAO; }

	explicit FORCEINLINE RawModel(const RawModel& other);
	FORCEINLINE RawModel& operator=(const RawModel& other);
private:
	// Utility Functions:
	template<ssize_t V, ssize_t T, ssize_t N, ssize_t I>
	void LoadFromArray(float(&vert)[V], float(&tex)[T], float(&normal)[N], uint32(&indices)[I]); 
	void LoadFromSettings(const RawModelSettings& settings);
	void LoadFromVector(const Vector<vec3>& vertices, const Vector<vec2>* textures, const Vector<vec3>* normals);

	VAO m_ModelVAO;
	VBO vertexVBO;
	VBO textureVBO;
	VBO normalVBO;
	VBO indexVBO;
	ssize_t m_VertexCount;
	const bool m_IsIndexed = IsIndexed;

	AUTO_CLEAN_WITH_CONTROL(RawModel);
};

template<>
template<ssize_t V, ssize_t T, ssize_t N>
RawModel<false>::RawModel(float(&vert)[V], float(&tex)[T], float(&normal)[N])
{
	LoadFromArray(vert, tex, normal);
	m_VertexCount = V / 3LLU; // Get the vertex Count!
	m_ModelVAO.Unuse();
}

template<>
template<ssize_t V, ssize_t T, ssize_t N, ssize_t I>
RawModel<true>::RawModel(float(&vert)[V], float(&tex)[T], float(&normal)[N], uint32(&indices)[I])
{
	LoadFromArray(vert, tex, normal);
	m_VertexCount = I; // Get the vertex Count!
	//Set up indices
	indexVBO.Generate(BufferTarget::ELEMENT_ARRAY_BUFFER);
	indexVBO.FillData(indices);
	m_ModelVAO.Unuse();
	indexVBO.Unuse();
}


template<>
template<ssize_t I>
RawModel<true>::RawModel(const RawModelSettings& settings, uint32(&indices)[I])
{
	ASSERTF(!(settings.vertexSize == 0 || settings.vertices == NULL), "Attempt to create a RawModel with empty vertecies!");
	LoadFromSettings(settings);
	m_VertexCount = I; // Get the vertex Count!
	//Set up indices
	indexVBO.Generate(BufferTarget::ELEMENT_ARRAY_BUFFER);
	indexVBO.FillData(indices);
	m_ModelVAO.Unuse();
	indexVBO.Unuse();
}

template<bool B>
FORCEINLINE void RawModel<B>::Use(const ShaderProgram& shader) const
{
	shader.Use();
	m_ModelVAO.Use();
}

template<bool B>
void RawModel<B>::LoadFromSettings(const RawModelSettings& settings)
{
	ASSERTF(!(settings.vertexSize == 0 || settings.vertices == NULL), "Attempt to create a RawModel with empty vertecies!");

	m_ModelVAO.Use();

	//Fill vertex:
	vertexVBO.Generate(BufferTarget::ARRAY_BUFFER);
	vertexVBO.FillData(settings.vertices, settings.vertexSize * sizeof(float));
	m_ModelVAO.BindAttribute<DataType::FLOAT>(0, vertexVBO, 3, 0, 0);

	if (settings.textureSize != 0 && settings.textures != NULL) { //Fill Texture if availble
		textureVBO.Generate(BufferTarget::ARRAY_BUFFER);
		textureVBO.FillData(settings.textures, settings.textureSize * sizeof(float));
		m_ModelVAO.BindAttribute<DataType::FLOAT>(1, textureVBO, 2, 0, 0);
	}

	if (settings.normalSize != 0 && settings.normals != NULL) { // Fill normals if availble
		//Fill normals:
		normalVBO.Generate(BufferTarget::ARRAY_BUFFER);
		normalVBO.FillData(settings.normals, settings.normalSize * sizeof(float));
		m_ModelVAO.BindAttribute<DataType::FLOAT>(2, normalVBO, 3, 0, 0);
		normalVBO.Unuse();
	}
}

template<>
FORCEINLINE void RawModel<true>::OnSetClean(bool b)
{
	m_ModelVAO.SetAutoClean(b);
	vertexVBO.SetAutoClean(b);
	textureVBO.SetAutoClean(b);
	normalVBO.SetAutoClean(b);
	indexVBO.SetAutoClean(b);
}

template<>
FORCEINLINE void RawModel<false>::OnSetClean(bool b)
{
	m_ModelVAO.SetAutoClean(b);
	vertexVBO.SetAutoClean(b);
	textureVBO.SetAutoClean(b);
	normalVBO.SetAutoClean(b);
}

template<bool B>
FORCEINLINE RawModel<B>::RawModel(const RawModel<B>& other) :
	m_ModelVAO(other.m_ModelVAO), vertexVBO(other.vertexVBO), textureVBO(other.textureVBO),
	normalVBO(other.normalVBO), indexVBO(other.indexVBO), m_VertexCount(other.m_VertexCount), m_AutoClean(true)
{
	const_cast<RawModel&>(other).SetAutoClean(false);
}

template<bool B>
FORCEINLINE RawModel<B>& RawModel<B>::operator=(const RawModel<B>& other) {
	const_cast<RawModel&>(other).SetAutoClean(false);
	this->SetAutoClean(true);
	m_ModelVAO = other.m_ModelVAO;
	vertexVBO = other.vertexVBO;
	textureVBO = other.textureVBO;
	normalVBO = other.normalVBO;
	indexVBO = other.indexVBO;
	m_VertexCount = other.m_VertexCount;
	return *this;
}

template<bool B>
RawModel<B>::~RawModel()
{
	if (m_AutoClean) {
		m_ModelVAO.Clean();
		vertexVBO.Clean();
		textureVBO.Clean();
		normalVBO.Clean();
		indexVBO.Clean();
	}
}

template<bool B>
void RawModel<B>::Clean()
{
	m_ModelVAO.Clean();
	vertexVBO.Clean();
	textureVBO.Clean();
	normalVBO.Clean();
	indexVBO.Clean();
}

TRE_NS_END