#pragma once

#include <Core/Misc/Defines/Common.hpp>
#include <Core/Misc/Singleton/Singleton.hpp>
#include <Core/DataStructure/Vector/Vector.hpp>
#include <Core/ECS/ECS/ECS.hpp>
#include <Renderer/Common/Common.hpp>
#include <Renderer/Backend/ContextOperationQueue/ContextOperationQueue.hpp>

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
	uint32 CreateResource(ResourcesTypes type, Args&&... args);

	template<typename T>
	T& Get(ResourcesTypes type, uint32 id);

	FORCEINLINE uint8* GetRawMemory(ResourcesTypes type, uint32 id, uint32 size);

	ContextOperationQueue& GetContextOperationsQueue(){ return m_OperationQueue; }

	World& GetRenderWorld() { return m_RenderWorld; }
private:
	Vector<uint8> m_MemoryPool[RESOURCES_COUNT];
	ContextOperationQueue m_OperationQueue;
	World& m_RenderWorld;
};

template<typename T, typename... Args>
uint32 ResourcesManager::CreateResource(ResourcesTypes type, Args&&... args)
{
	uint32 type_index = (uint32)type;
	uint32 id = (uint32) m_MemoryPool[type_index].Size();
	m_MemoryPool[type_index].Resize(id + sizeof(T));
	new (&m_MemoryPool[type_index][id]) T(std::forward<Args>(args)...);
	return id / sizeof(T);
}

template<typename T>
T& ResourcesManager::Get(ResourcesTypes type, uint32 id)
{
	return *reinterpret_cast<T*>(GetRawMemory(type, id, sizeof(T)));
}

FORCEINLINE uint8* ResourcesManager::GetRawMemory(ResourcesTypes type, uint32 id, uint32 size)
{
	return &(m_MemoryPool[(uint32)type][id * size]);
}

TRE_NS_END
