template<typename SuperClass, typename T, typename... Args>
ICommandBuffer<SuperClass, T, Args...>::ICommandBuffer(void(SuperClass::*submit_func)(Args...), uint32 mem_multiplier, uint32 aux_memory)
    : 
    m_KeyPacketPtrAllocator(DEFAULT_SIZE * mem_multiplier + aux_memory, true),
    m_CmdAllocator(DEFAULT_MAX_ELEMENTS * DEFAULT_MAX_ELEMENTS * mem_multiplier, true),
    m_Keys(NULL), m_Packets(NULL),
    m_SubmitFunc(submit_func), m_Current(0), m_PacketCount(0)
{
    usize total_mem = DEFAULT_SIZE * mem_multiplier;
    uint8* free_mem = (uint8*) m_KeyPacketPtrAllocator.Allocate(total_mem);
    m_Packets = (void**) free_mem;
    m_Keys    = (Pair<Key, uint32>*) (free_mem + (sizeof(void**) * DEFAULT_MAX_ELEMENTS * mem_multiplier));
}

template<typename SuperClass, typename T, typename... Args>
template<typename U>
U* ICommandBuffer<SuperClass, T, Args...>::AddCommand(T key , usize aux_memory)
{
    CmdPacket packet = CommandPacket::Create<U>(m_CmdAllocator, aux_memory);
    // store key and pointer to the data
    {
        const uint32 current = m_Current;

        // if (current >= DEFAULT_MAX_ELEMENTS)
        //    return (U*) m_Packets[current - 1];

        new (m_Keys + current) Pair<Key, uint32>(key, current);
        m_Packets[current] = packet;
        m_Current++;
        m_PacketCount++;
    }
    
    CommandPacket::StoreNextCommandPacket(packet, NULL);
    CommandPacket::StoreBackendDispatchFunction(packet, U::DISPATCH_FUNCTION);
 
    return CommandPacket::GetCommand<U>(packet);
}

template<typename SuperClass, typename T, typename... Args>
template <typename U, typename V>
U* ICommandBuffer<SuperClass, T, Args...>::AppendCommand(V* command, usize aux_memory)
{
    CmdPacket packet = CommandPacket::Create<U>(m_CmdAllocator, aux_memory);
 
    // append this command to the given one
    CommandPacket::StoreNextCommandPacket<V>(command, packet);
 
    CommandPacket::StoreNextCommandPacket(packet, NULL);
    CommandPacket::StoreBackendDispatchFunction(packet, U::DISPATCH_FUNCTION);
 
    return CommandPacket::GetCommand<U>(packet);
}

template<typename SuperClass, typename T, typename... Args>
FORCEINLINE void ICommandBuffer<SuperClass, T, Args...>::Sort()
{
    const uint32 current = (m_Current - m_PacketCount);
    std::qsort(m_Keys + current, m_PacketCount, sizeof(Pair<Key, uint32>), [](const void* a, const void* b){
        const Pair<Key, uint32>& arg1 = *static_cast<const Pair<Key, uint32>*>(a);
        const Pair<Key, uint32>& arg2 = *static_cast<const Pair<Key, uint32>*>(b);
 
        if(arg1.first < arg2.first) return -1;
        if(arg1.first > arg2.first) return 1;
        return 0;

    });

    /*for (uint32 i = current; i < current + m_PacketCount; i++) {
        printf("Key = %lu\n", m_Keys[i].first);
    }*/
}

template<typename SuperClass, typename T, typename... Args>
void ICommandBuffer<SuperClass, T, Args...>::SubmitPacket(const CmdPacket packet) const
{
    const BackendDispatchFunction CommandFunction = CommandPacket::LoadBackendDispatchFunction(packet);
    const void* command = CommandPacket::LoadCommand(packet);
    CommandFunction(command);
}

template<typename SuperClass, typename T, typename... Args>
FORCEINLINE void ICommandBuffer<SuperClass, T, Args...>::Submit(const Args&... args)
{   
    void(*func)(Args...) = reinterpret_cast<void(SuperClass::*)(Args...)>(m_SubmitFunc);
    return func(args...);
}

template<typename SuperClass, typename T, typename... Args>
FORCEINLINE void ICommandBuffer<SuperClass, T, Args...>::Reset()
{
    m_Current = 0;
    m_PacketCount = 0;
    m_CmdAllocator.Reset();
}
