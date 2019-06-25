#include "Material.hpp"

TRE_NS_START

void Material::AddTexture(const TextureMap& type, const char* tex_path)
{
	printf("Loading the texture : %s\n", tex_path);
	m_Textures.Emplace(
		type, 
		Texture(tex_path, TexTarget::TEX2D, {
			{TexParam::TEX_WRAP_S , TexWrapping::REPEAT},
			{TexParam::TEX_WRAP_T, TexWrapping::REPEAT},
			{TexParam::TEX_MIN_FILTER, TexFilter::LINEAR},
			{TexParam::TEX_MAG_FILTER, TexFilter::LINEAR}
		})
	);
}

TRE_NS_END