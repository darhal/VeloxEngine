#pragma once

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

class MeshLoader
{
public:
	MeshLoader(const char* path);
	void LoadFile(const char* path);
private:
	Vector<vec4> m_Verticies;
	Vector<vec3> m_Normals;
	Vector<vec3> m_TextureCoord;
	Vector<vec3> m_Faces;
};

int32 ParseUint64(char* str, uint64* x);

TRE_NS_END