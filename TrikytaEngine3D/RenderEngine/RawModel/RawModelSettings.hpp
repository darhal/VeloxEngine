#pragma once

#include <Core/Misc/Defines/Common.hpp>
#include <type_traits>

struct RawModelSettings
{
	enum VERTEX_DATA {
		VERTEX,
		TEXTURE,
		NORMAL,
	};

	ssize_t vertexSize;
	const float* vertices;

	ssize_t textureSize;
	const float* textures;

	ssize_t normalSize;
	const float* normals;

	FORCEINLINE RawModelSettings() : vertexSize(0), textureSize(0), normalSize(0), vertices(NULL), textures(NULL), normals(NULL)
	{}

	template<ssize_t V, ssize_t T, ssize_t N>
	FORCEINLINE RawModelSettings(float(&vert)[V], float(&tex)[T], float(&normal)[N]) : vertexSize(V), textureSize(T), normalSize(N), vertices(vert), textures(tex), normals(normal)
	{}

	template<ssize_t V>
	FORCEINLINE RawModelSettings(float(&vert)[V]) : vertexSize(V), textureSize(0), normalSize(0), vertices(vert), textures(NULL), normals(NULL)
	{}

	template<ssize_t V, ssize_t T>
	FORCEINLINE RawModelSettings(float(&vert)[V], float(&tex)[T]) : vertexSize(V), textureSize(T), vertices(vert), textures(tex), normalSize(0), normals(NULL)
	{}

	FORCEINLINE RawModelSettings(const float* vert, ssize vertSize, const float* tex = NULL, ssize texSize = 0, const float* normal = NULL, ssize normalSize = 0) :
		vertexSize(vertexSize), textureSize(texSize), normalSize(normalSize),
		vertices(vert), textures(tex), normals(normal)
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
};