#pragma once

#include "Core/Misc/Defines/Common.hpp"
#include "RenderEngine/Renderer/Backend/CommandBuffer/ICommandBuffer/ICommandBuffer.hpp"

TRE_NS_START

template<typename T>
class ResourcesCommandBucket : public ICommandBuffer<ResourcesCommandBucket<T>, T>
{
public:
    typedef T Key;
    typedef ICommandBuffer<ResourcesCommandBucket<T>, T> BaseClass;

	ResourcesCommandBucket();

    void Submit();

	void Clear();

	bool SwapCmdBuffer();

private:
	/*uint32 m_StartLocation, m_SecondCurrent;
	std::mutex mtx;
	std::condition_variable cv;
	bool m_IsReading;*/
};

template<typename T>
using ResourcesCommandBuffer = ResourcesCommandBucket<T>;

template<typename T>
ResourcesCommandBucket<T>::ResourcesCommandBucket() : 
	BaseClass(&ResourcesCommandBucket<T>::Submit, 2)
	/*m_StartLocation(BaseClass::DEFAULT_MAX_ELEMENTS),
	m_SecondCurrent(BaseClass::DEFAULT_MAX_ELEMENTS),
	mtx(),
	cv(),
	m_IsReading(false)*/
{
	//m_IsReading = 1;
	//cv.notify_one();
}

template<typename T>
void ResourcesCommandBucket<T>::Submit()
{
	const uint32 max = BaseClass::m_Current;
	const uint32 start = 0;

    if (max != start){

		for (uint32 i = start; i < max; i++) {
            const Pair<Key, uint32>& k = BaseClass::m_Keys[i];
            CmdPacket packet = BaseClass::m_Packets[k.second];

            do{
                this->SubmitPacket(packet);
                packet = CommandPacket::LoadNextCommandPacket(packet);
            } while (packet != NULL);
        }
        
        BaseClass::Reset();
		//m_SecondCurrent = m_StartLocation;
    }

	//m_IsReading = 0;
	//cv.notify_one();
}

template<typename T>
void ResourcesCommandBucket<T>::Clear()
{
	/*if (m_StartLocation) {
		BaseClass::m_Current = 0;
	}else {
		BaseClass::m_Current = BaseClass::DEFAULT_MAX_ELEMENTS;
	}

	BaseClass::m_CmdAllocator.SetOffset(BaseClass::m_Current * BaseClass::m_Current);
	BaseClass::m_PacketCount = 0;*/
}

template<typename T>
bool ResourcesCommandBucket<T>::SwapCmdBuffer()
{
	/*std::unique_lock<std::mutex> lk(mtx);
	cv.wait(lk, [this] {return !m_IsReading; });

	m_SecondCurrent = BaseClass::m_Current;

	if (m_StartLocation) {
		m_StartLocation = 0;
	}else {
		m_StartLocation = BaseClass::DEFAULT_MAX_ELEMENTS;
	}

	m_IsReading = 1;
	return true;*/
}


TRE_NS_END