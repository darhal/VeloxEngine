#pragma once

#include <Core/Misc/Defines/Common.hpp>
#include <Core/Misc/Utils/Common.hpp>
#include <Core/Misc/Maths/Common.hpp>
#include "MaterialLoader.hpp"
#include <Core/DataStructure/HashMap/Map.hpp>
#include <Core/DataStructure/String/String.hpp>
#include <RenderEngine/Mesh/ModelLoader/ModelLoader.hpp>

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

	void ProcessData(Vector<ModelLoader>& arrayOfObjects);

 	MaterialLoader& GetMaterialLoader() { return m_MaterialLoader; };
	
	ModelLoader LoadAsOneObject();

	ModelLoader LoadObject(const String& name);
private:
	Map<uint8, Vector<vec3>> m_Verticies;
	Map<uint8, Vector<vec3>> m_Normals;
	Map<uint8, Vector<vec2>> m_TextureCoord;
	Map<uint8, Vector<uint32>> m_NormalIndicies;
	Map<String, uint8> m_Objects;
	Map<uint8, Vector<MatrialForRawModel>> m_Materials;
	MaterialLoader m_MaterialLoader;
	uint8 m_ObjectCount;

	Map<uint8, Map<std::tuple<ssize, ssize, ssize>, uint32>> m_VertexDataIndex;
	Map<uint8, uint32> m_IndiciesOfVData;
	Map<uint8, Vector<uint32>> m_DataIndex;
	Map<uint8, Vector<VertexData>> m_VerteciesData;
};

TRE_NS_END