#include "AbstractMaterial.hpp"
#include <Core/Misc/Utils/Image.hpp>
#include "RenderEngine/Managers/RenderManager/RenderManager.hpp"

TRE_NS_START

TextureID AbstractMaterial::AddTexture(const String& path)
{
    auto& res_buffer = RenderManager::GetRenderer().GetResourcesCommandBuffer();
    Image img(path.Buffer());
    TextureID tex_id = 0;

    Commands::CreateTexture* tex_cmd = res_buffer.AddCommand<Commands::CreateTexture>(0);
    tex_cmd->texture = ResourcesManager::GetGRM().Create<Texture>(tex_id);
    tex_cmd->settings = TextureSettings(TexTarget::TEX2D, img.GetWidth(), img.GetHeight(), img.StealPtr(), 
      	Vector<TexParamConfig>{
			{TexParam::TEX_WRAP_S , TexWrapping::REPEAT},
			{TexParam::TEX_WRAP_T, TexWrapping::REPEAT},
			{TexParam::TEX_MIN_FILTER, TexFilter::LINEAR},
			{TexParam::TEX_MAG_FILTER, TexFilter::LINEAR}
		}
    );
        
    return tex_id;
}

TRE_NS_END