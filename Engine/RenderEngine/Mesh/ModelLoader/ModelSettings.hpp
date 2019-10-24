#pragma once

#include <Core/Misc/Defines/Common.hpp>
#include <type_traits>
#include <cstring>

struct ModelSettings
{
	enum VERTEX_DATA {
		VERTEX,
		TEXTURE,
		NORMAL,
		INDEX,
	};

	uint32 vertexSize = 0;
	uint32 textureSize = 0;
	uint32 normalSize = 0;
	uint32 indexSize = 0;

	float* vertices = NULL;
	float* textures = NULL;
	float* normals = NULL;
	uint32* indices = NULL;

	FORCEINLINE ModelSettings() 
		: vertexSize(0), textureSize(0), normalSize(0), indexSize(0), 
		vertices(NULL), textures(NULL),  normals(NULL), indices(NULL)
	{}

	template<uint32 V, uint32 T, uint32 N, uint32 I>
	FORCEINLINE ModelSettings(float(&vert)[V], float(&tex)[T], float(&normal)[N], uint32(&indices)[I]) 
		: vertexSize(V), textureSize(T), normalSize(N), indexSize(I), 
		vertices(vert), textures(tex), normals(normal), indices(indices)
	{}

	template<uint32 V>
	FORCEINLINE ModelSettings(float(&vert)[V]) 
		: vertexSize(V), normalSize(0), textureSize(0), indexSize(0), 
		vertices(vert), textures(NULL), normals(NULL), indices(NULL)
	{}

	template<uint32 V, uint32 T>
	FORCEINLINE ModelSettings(float(&vert)[V], float(&tex)[T]) : 
		vertexSize(V), textureSize(T), normalSize(0), indexSize(0), 
		vertices(vert), textures(tex), normals(NULL), indices(NULL)
	{}

	FORCEINLINE ModelSettings(float* vert, uint32 vertSize, float* tex = NULL, uint32 texSize = 0, float* normal = NULL, uint32 normalSize = 0, uint32* indices = NULL, uint32 inSize = 0) :
		vertexSize(vertSize), textureSize(texSize), normalSize(normalSize), indexSize(inSize),
		vertices(vert), textures(tex), normals(normal), indices(indices)
	{}

	template<VERTEX_DATA VD, typename std::enable_if<VD == VERTEX, bool>::type = true, ssize_t V>
	FORCEINLINE void Set(float(&vert)[V])
	{
		vertexSize = V;
		vertices = vert;
	}

	template<VERTEX_DATA VD, typename std::enable_if<VD == NORMAL, bool>::type = true, ssize_t N>
	FORCEINLINE void Set(float(&normal)[N])
	{
		normalSize = N;
		normals = normal;
	}

	template<VERTEX_DATA VD, typename std::enable_if<VD == TEXTURE, bool>::type = true, ssize_t T>
	FORCEINLINE void Set(float(&tex)[T])
	{
		textureSize = T;
		textures = tex;
	}

	template<VERTEX_DATA VD, typename std::enable_if<VD == INDEX, bool>::type = true, ssize_t I>
	FORCEINLINE void Set(uint32(&ind)[I])
	{
		indexSize = I;
		indices = ind;
	}

	void CopyData()
	{
		if (vertexSize && vertices) {
			float* v_data = new float[vertexSize * sizeof(float)];
			memcpy(v_data, vertices, vertexSize * sizeof(float));
			vertices = v_data;
		}
		
		if (textureSize && textures) {
			float* t_data = new float[textureSize * sizeof(float)];
			memcpy(t_data, textures, textureSize * sizeof(float));
			textures = t_data;
		}

		if (normalSize && normals) {
			float* n_data = new float[normalSize * sizeof(float)];
			memcpy(n_data, normals, normalSize * sizeof(float));
			normals = n_data;
		}

		if (indexSize && indices) {
			uint32* i_data = new uint32[indexSize * sizeof(uint32)];
			memcpy(i_data, indices, indexSize * sizeof(uint32));
			indices = i_data;
		}
	}
};