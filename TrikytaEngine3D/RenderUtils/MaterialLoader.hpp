#pragma once

#include <Core/Misc/Defines/Common.hpp>
#include <Core/Misc/Utils/Common.hpp>
#include <RenderEngine/Materials/Material.hpp>

TRE_NS_START

class MaterialLoader
{
public:
	MaterialLoader();
	void LoadFileMTL(const char* path);
	Material& GetMaterialFromName(const char* name);
private:
	HashMap<String, Material> m_NameToMaterial;
};

TRE_NS_END