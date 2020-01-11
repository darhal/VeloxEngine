#pragma once

#include <Core/Misc/Defines/Common.hpp>

struct ModelData
{
	float* vertices = NULL;
	float* textures = NULL;
	float* normals = NULL;
	uint32* indices = NULL;

	uint32 vertexSize = 0;
	uint32 textureSize = 0;
	uint32 normalSize = 0;
	uint32 indexSize = 0;
};