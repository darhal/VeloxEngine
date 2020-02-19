#include "AbstractMaterial.hpp"
#include <Core/Misc/Utils/Image.hpp>
#include <Renderer/Backend/ResourcesManager/ResourcesManager.hpp>
#include <Renderer/Backend/Commands/Commands.hpp>

TRE_NS_START

TextureID AbstractMaterial::AddTexture(const String& path)
{
    Image img(path.Buffer());
	ResourcesManager& manager = ResourcesManager::Instance();
	ContextOperationQueue& op_queue = manager.GetContextOperationsQueue();
    TextureID tex_id = manager.CreateResource<Texture>();

    Commands::CreateTextureCmd* tex_cmd = op_queue.SubmitCommand<Commands::CreateTextureCmd>();
    tex_cmd->texture = &manager.Get<Texture>(tex_id);
    tex_cmd->settings = TextureSettings(TexTarget::TEX2D, img.GetWidth(), img.GetHeight(), img.StealPtr(), 
      	Vector<TexParamConfig>{
			{TexParam::TEX_WRAP_S , TexWrapping::REPEAT},
			{TexParam::TEX_WRAP_T, TexWrapping::REPEAT },
			{TexParam::TEX_MIN_FILTER, TexFilter::LINEAR},
			{TexParam::TEX_MAG_FILTER, TexFilter::LINEAR}
		}
    );
   
    return tex_id;
}

TRE_NS_END