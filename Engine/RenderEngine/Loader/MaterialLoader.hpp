#pragma once

#include <Core/Misc/Defines/Common.hpp>
#include <Core/Misc/Utils/Common.hpp>
#include "RenderEngine/Materials/AbstractMaterial/AbstractMaterial.hpp"
#include <Core/DataStructure/HashMap/Map.hpp>

TRE_NS_START

class MaterialLoader
{
public:
	MaterialLoader();

	void LoadFileMTL(const char* mtrl_path, const char* obj_path);

	AbstractMaterial& GetMaterialFromName(const char* name);

	Map<String, AbstractMaterial>& GetMaterials();
	
private:
	Map<String, AbstractMaterial> m_NameToMaterial;
};

TRE_NS_END