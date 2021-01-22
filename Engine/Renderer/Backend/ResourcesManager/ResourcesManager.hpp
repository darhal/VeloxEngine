#pragma once

#include <Core/Misc/Defines/Common.hpp>
#include <Core/Misc/Singleton/Singleton.hpp>
#include <Core/DataStructure/Vector/Vector.hpp>
#include <Core/ECS/ECS/ECS.hpp>
#include <Renderer/Common/Common.hpp>
#include <Renderer/Backend/ContextOperationQueue/ContextOperationQueue.hpp>
#include "ResourcesMeta/ResourcesMetadata.hpp"

TRE_NS_START

class ResourcesManager : public Singleton<ResourcesManager>
{
public:
	ResourcesManager() : m_RenderWorld(ECS::CreateWorld())
	{
		for (uint32 i = 0; i < RESOURCES_COUNT; i++)
			m_MemoryPool[i].Reserve(1024);
	}

	template<typename T, typename... Args>
	uint32 AllocateResource(Args&&... args);

	template<typename T, typename... Args>
	T& CreateResource(uint32& res_id, Args&&... args);

	template<typename T, typename... Args>
	Commands::CreateResourceCmd<T>* Create(uint32& res_id, Args&&... args);

	template<typename T>
	T& Get(uint32 id);

	FORCEINLINE uint8* GetRawMemory(ResourcesTypes type, uint32 id, uint32 size);

	ContextOperationQueue& GetContextOperationsQueue(){ return m_OperationQueue; }

	World& GetRenderWorld() { return m_RenderWorld; }

private:
	Vector<uint8> m_MemoryPool[RESOURCES_COUNT];
	ContextOperationQueue m_OperationQueue;
	World& m_RenderWorld;
};

template<typename T, typename... Args>
uint32 ResourcesManager::AllocateResource(Args&&... args)
{
	uint32 type_index = (uint32)ResourcesInfo<T>::RESOURCE_TYPE;
	uint32 id = (uint32) m_MemoryPool[type_index].Size();
	m_MemoryPool[type_index].Resize(id + sizeof(T));
	new (&m_MemoryPool[type_index][id]) T(std::forward<Args>(args)...);
	return id / sizeof(T);
}

template<typename T>
T& ResourcesManager::Get(uint32 id)
{
	return *reinterpret_cast<T*>(GetRawMemory(ResourcesInfo<T>::RESOURCE_TYPE, id, sizeof(T)));
}

FORCEINLINE uint8* ResourcesManager::GetRawMemory(ResourcesTypes type, uint32 id, uint32 size)
{
	return &(m_MemoryPool[(uint32)type][id * size]);
}


template<typename T, typename... Args>
Commands::CreateResourceCmd<T>* ResourcesManager::Create(uint32& res_id, Args&&... args)
{
	res_id = this->AllocateResource<T>(std::forward<Args>(args)...);
	Commands::CreateResourceCmd<T>* cmd = m_OperationQueue.SubmitCommand<Commands::CreateResourceCmd<T>>();
	cmd->resource = &this->Get<T>(res_id);
	return cmd;
}

template<typename T, typename... Args>
T& ResourcesManager::CreateResource(uint32& res_id, Args&&... args)
{
	res_id = this->AllocateResource<T>(std::forward<Args>(args)...);
	return this->Get<T>(res_id);;
}


TRE_NS_END
