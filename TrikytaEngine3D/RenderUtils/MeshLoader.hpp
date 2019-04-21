#pragma once

#include <RenderEngine/RawModel/RawModel.hpp>
#include <Core/Misc/Defines/Common.hpp>
#include <Core/Misc/Utils/Common.hpp>
#include <Core/Misc/Maths/Common.hpp>
#include "MaterialLoader.hpp"
#include <RenderEngine/RawModel/RawModel.hpp>

TRE_NS_START

namespace MeshFormat
{
	enum mesh_formart_t {
		OBJ,
	};
}

class VAO;
class VBO;
class MaterialLoader;

class MeshLoader
{
public:
	MeshLoader(const char* path);
	void LoadFile(const char* path);
	void ProcessData(Vector<RawModel<true>>* arrayOfObjects);
private:
	HashMap<uint8, Vector<vec3>> m_Verticies;
	HashMap<uint8, Vector<vec3>> m_Normals;
	HashMap<uint8, Vector<vec2>> m_TextureCoord;
	HashMap<uint8, Vector<uint32>> m_Indicies;
	HashMap<uint8, String> m_Objects;
	HashMap<uint8, Vector<MatrialForRawModel>> m_Materials;
	MaterialLoader m_MaterialLoader;
	uint8 m_ObjectCount;
};

TRE_NS_END