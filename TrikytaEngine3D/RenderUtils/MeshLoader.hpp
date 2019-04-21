#pragma once

#include <RenderEngine/RawModel/RawModel.hpp>
#include <Core/Misc/Defines/Common.hpp>
#include <Core/Misc/Utils/Common.hpp>
#include <Core/Misc/Maths/Common.hpp>


TRE_NS_START

namespace MeshFormat
{
	enum mesh_formart_t {
		OBJ,
	};
}

class VAO;
class VBO;

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
	uint8 m_ObjectCount;
};

int32 ParseUint64(char* str, uint64* x);

TRE_NS_END