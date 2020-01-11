#pragma once

#include <Core/Misc/Defines/Common.hpp>
#include <Renderer/Materials/AbstractMaterial/AbstractMaterial.hpp>

TRE_NS_START

struct ModelMaterialData
{
	const AbstractMaterial& material;
	uint32 vcount;

	ModelMaterialData(const AbstractMaterial& material, uint32 vcount) : material(material), vcount(vcount)
	{}

	ModelMaterialData(const ModelMaterialData& other) : material(other.material), vcount(other.vcount)
	{}
};

TRE_NS_END