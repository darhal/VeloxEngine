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

	ssize_t vertexSize = 0;
	float* vertices = NULL;

	ssize_t textureSize = 0;
	float* textures = NULL;

	ssize_t normalSize = 0;
	float* normals = NULL;

	ssize_t indexSize = 0;
	uint32* indices = NULL;

	FORCEINLINE ModelSettings() 
		: vertexSize(0), vertices(NULL), textureSize(0), textures(NULL), normalSize(0), normals(NULL), indexSize(0), indices(NULL)
	{}

	template<ssize_t V, ssize_t T, ssize_t N, ssize_t I>
	FORCEINLINE ModelSettings(float(&vert)[V], float(&tex)[T], float(&normal)[N], uint32(&indices)[I]) 
		: vertexSize(V), vertices(vert), textureSize(T), textures(tex), normalSize(N), normals(normal), indexSize(I), indices(indices)
	{}

	template<ssize_t V>
	FORCEINLINE ModelSettings(float(&vert)[V]) 
		: vertexSize(V), vertices(vert), textureSize(0), textures(NULL), normalSize(0), normals(NULL), indexSize(0), indices(NULL)
	{}

	template<ssize_t V, ssize_t T>
	FORCEINLINE ModelSettings(float(&vert)[V], float(&tex)[T]) : 
		vertexSize(V), vertices(vert), textureSize(T), textures(tex), normalSize(0), normals(NULL), indexSize(0), indices(NULL)
	{}

	FORCEINLINE ModelSettings(float* vert, ssize vertSize, float* tex = NULL, ssize texSize = 0, float* normal = NULL, ssize normalSize = 0, uint32* indices = NULL, ssize_t inSize = 0) :
		vertexSize(vertSize), vertices(vert),
		textureSize(texSize), textures(tex),
		normalSize(normalSize), normals(normal),
		indexSize(inSize), indices(indices)
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