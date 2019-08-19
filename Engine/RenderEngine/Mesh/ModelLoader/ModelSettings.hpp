#pragma once

#include <Core/Misc/Defines/Common.hpp>
#include <type_traits>

struct ModelSettings
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

	FORCEINLINE ModelSettings() : vertexSize(0), vertices(NULL), textureSize(0), textures(NULL), normalSize(0), normals(NULL)
	{}

	template<ssize_t V, ssize_t T, ssize_t N>
	FORCEINLINE ModelSettings(float(&vert)[V], float(&tex)[T], float(&normal)[N]) : vertexSize(V), vertices(vert), textureSize(T), textures(tex), normalSize(N), normals(normal)
	{}

	template<ssize_t V>
	FORCEINLINE ModelSettings(float(&vert)[V]) : vertexSize(V), vertices(vert), textureSize(0), textures(NULL), normalSize(0), normals(NULL)
	{}

	template<ssize_t V, ssize_t T>
	FORCEINLINE ModelSettings(float(&vert)[V], float(&tex)[T]) : vertexSize(V), vertices(vert), textureSize(T), textures(tex), normalSize(0), normals(NULL)
	{}

	FORCEINLINE ModelSettings(const float* vert, ssize vertSize, const float* tex = NULL, ssize texSize = 0, const float* normal = NULL, ssize normalSize = 0) :
		vertexSize(vertSize), vertices(vert),
		textureSize(texSize), textures(tex),
		normalSize(normalSize), normals(normal)
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