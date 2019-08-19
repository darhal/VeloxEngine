#pragma once

#include <stdio.h>
#include <algorithm>

#include "Core/Misc/Defines/Common.hpp"
#include "RenderEngine/Renderer/Backend/CommandPacket/CommandPacket.hpp"
#include "Core/Memory/Allocators/LinearAlloc/LinearAllocator.hpp"

TRE_NS_START

template<typename SuperClass, typename T, typename... Args>
class ICommandBuffer
{
public:
    typedef T Key;
    
    ICommandBuffer(void(SuperClass::*submit_func)(Args...), uint32 mem_multiplier = 1) ;
    /*[----------------------------------------------------------------------------------]*/
    /* |                        |                  |                     |                */
    /* start of packet          start of packet 2  Start of key          Start of key2    */

    template<typename U>
    U* AddCommand(Key key, usize aux_memory = 0);

    template <typename U, typename V>
    U* AppendCommand(V* command, usize aux_memory = 0);
    
    FORCEINLINE void Sort();

    FORCEINLINE void Submit(const Args&... args);

    FORCEINLINE void Reset();

protected:
    FORCEINLINE void SubmitPacket(const CmdPacket packet) const;

protected:
    LinearAlloc m_KeyPacketPtrAllocator;
    LinearAlloc m_CmdAllocator;
    Pair<Key, uint32>* m_Keys;
    void** m_Packets;
    void(SuperClass::*m_SubmitFunc)(Args...);
    uint32 m_Current, m_PacketCount;

    CONSTEXPR static uint32 DEFAULT_MAX_ELEMENTS = 1023;
    CONSTEXPR static uint32 DEFAULT_SIZE         = DEFAULT_MAX_ELEMENTS * (sizeof(Pair<Key, uint32>) + sizeof(void**));
};

#include "ICommandBuffer.inl"

TRE_NS_END