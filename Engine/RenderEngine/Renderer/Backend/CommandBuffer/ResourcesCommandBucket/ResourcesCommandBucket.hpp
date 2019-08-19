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

    ResourcesCommandBucket() : BaseClass(&ResourcesCommandBucket::Submit)
    {};

    FORCEINLINE void Submit();

private:

};

template<typename T>
using ResourcesCommandBuffer = ResourcesCommandBucket<T>;

template<typename T>
FORCEINLINE void ResourcesCommandBucket<T>::Submit()
{
    if (BaseClass::m_Current){

        for(uint32 i = 0; i < BaseClass::m_Current; i++){
            const Pair<Key, uint32>& k = BaseClass::m_Keys[i];
            CmdPacket packet = BaseClass::m_Packets[k.second];

            do{
                this->SubmitPacket(packet);
                packet = CommandPacket::LoadNextCommandPacket(packet);
            } while (packet != NULL);
        }
        
        BaseClass::Reset();
    }
}



TRE_NS_END