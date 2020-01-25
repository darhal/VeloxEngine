#include "LightSystem.hpp"
#include <Renderer/Backend/ResourcesManager/ResourcesManager.hpp>
#include <Renderer/Backend/Commands/Commands.hpp>

TRE_NS_START

LightSystem::LightSystem() : m_LightCount(0)
{
}

void LightSystem::AddLight(Mat4f* comp)
{
	ResourcesManager& manager = ResourcesManager::Instance();

	{
		Commands::EditSubBufferCmd* edit_sub_buff = manager.GetContextOperationsQueue().SubmitCommand<Commands::EditSubBufferCmd>();
		edit_sub_buff->vbo = m_LightVbo;
		edit_sub_buff->data = comp;
		edit_sub_buff->offset = m_LightCount * sizeof(Mat4f);
		edit_sub_buff->size = sizeof(Mat4f);
	}
	{
		m_LightCount++;
		Commands::EditSubBufferCmd* edit_sub_buff = manager.GetContextOperationsQueue().SubmitCommand<Commands::EditSubBufferCmd>();
		edit_sub_buff->vbo = m_LightVbo;
		edit_sub_buff->data = &m_LightCount;
		edit_sub_buff->offset = sizeof(Mat4f) * m_MaxLight;
		edit_sub_buff->size = sizeof(uint32);
	}
}


TRE_NS_END
