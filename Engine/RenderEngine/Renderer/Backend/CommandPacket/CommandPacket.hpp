#pragma once

#include "Core/Misc/Defines/Common.hpp"
#include "RenderEngine/Renderer/Backend/Commands/Commands.hpp"

TRE_NS_START

typedef void* CmdPacket;
typedef CmdPacket Command;

class CommandPacket
{
public:
    CONSTEXPR static usize OFFSET_NEXT_COMMAND_PACKET = 0;
    CONSTEXPR static usize OFFSET_BACKEND_DISPATCH_FUNCTION = OFFSET_NEXT_COMMAND_PACKET + sizeof(CmdPacket);
    CONSTEXPR static usize OFFSET_COMMAND = OFFSET_BACKEND_DISPATCH_FUNCTION + sizeof(BackendDispatchFunction);
 
    template <typename T>
    FORCEINLINE static CmdPacket Create(BaseAllocator& alloc, usize auxMemorySize);

    template <typename T>
    FORCEINLINE static usize GetSize(usize auxMemorySize);
 
    FORCEINLINE static CmdPacket* GetNextCommandPacket(CmdPacket packet);
 
    template <typename T>
    FORCEINLINE static CmdPacket* GetNextCommandPacket(T* command);
 
    FORCEINLINE static BackendDispatchFunction* GetBackendDispatchFunction(CmdPacket packet);
 
    template <typename T>
    FORCEINLINE static T* GetCommand(CmdPacket packet);

    template <typename T>
    FORCEINLINE static char* GetAuxiliaryMemory(T* command);
 
    FORCEINLINE static void StoreNextCommandPacket(CmdPacket packet, CmdPacket nextPacket);
 
    template <typename T>
    FORCEINLINE static void StoreNextCommandPacket(T* command, CmdPacket nextPacket);

    FORCEINLINE static void StoreBackendDispatchFunction(CmdPacket packet, BackendDispatchFunction dispatchFunction);
 
    FORCEINLINE static const CmdPacket LoadNextCommandPacket(const CmdPacket packet);
 
    FORCEINLINE static const BackendDispatchFunction LoadBackendDispatchFunction(const CmdPacket packet);
 
    FORCEINLINE static const void* LoadCommand(const CmdPacket packet);
};

/******************** STATIC FUNCTION ************************/

template <typename T>
FORCEINLINE CmdPacket CommandPacket::Create(BaseAllocator& alloc, usize auxMemorySize)
{
    CmdPacket adr =  (CmdPacket) alloc.Allocate(CommandPacket::GetSize<T>(auxMemorySize));
    return adr;
}
 
template <typename T>
FORCEINLINE usize CommandPacket::GetSize(usize auxMemorySize)
{
    return OFFSET_COMMAND + sizeof(T) + auxMemorySize;
}
 
FORCEINLINE CmdPacket* CommandPacket::GetNextCommandPacket(CmdPacket packet)
{
    return reinterpret_cast<CmdPacket*>(reinterpret_cast<char*>(packet) + OFFSET_NEXT_COMMAND_PACKET);
}
 
template <typename T>
FORCEINLINE CmdPacket* CommandPacket::GetNextCommandPacket(T* command)
{
    return reinterpret_cast<CmdPacket*>(reinterpret_cast<char*>(command) - OFFSET_COMMAND + OFFSET_NEXT_COMMAND_PACKET);
}
 
FORCEINLINE BackendDispatchFunction* CommandPacket::GetBackendDispatchFunction(CmdPacket packet)
{
    return reinterpret_cast<BackendDispatchFunction*>(reinterpret_cast<char*>(packet) + OFFSET_BACKEND_DISPATCH_FUNCTION);
}
 
template <typename T>
FORCEINLINE T* CommandPacket::GetCommand(CmdPacket packet)
{
    return reinterpret_cast<T*>(reinterpret_cast<char*>(packet) + OFFSET_COMMAND);
}
 
template <typename T>
FORCEINLINE char* CommandPacket::GetAuxiliaryMemory(T* command)
{
    return reinterpret_cast<char*>(command) + sizeof(T);
}
 
FORCEINLINE void CommandPacket::StoreNextCommandPacket(CmdPacket packet, CmdPacket nextPacket)
{
    *CommandPacket::GetNextCommandPacket(packet) = nextPacket;
}
 
template <typename T>
FORCEINLINE void CommandPacket::StoreNextCommandPacket(T* command, CmdPacket nextPacket)
{
    *CommandPacket::GetNextCommandPacket<T>(command) = nextPacket;
}
 
FORCEINLINE void CommandPacket::StoreBackendDispatchFunction(CmdPacket packet, BackendDispatchFunction dispatchFunction)
{
    *CommandPacket::GetBackendDispatchFunction(packet) = dispatchFunction;
}
 
FORCEINLINE const CmdPacket CommandPacket::LoadNextCommandPacket(const CmdPacket packet)
{
    return *GetNextCommandPacket(packet);
}
 
FORCEINLINE const BackendDispatchFunction CommandPacket::LoadBackendDispatchFunction(const CmdPacket packet)
{
    return *GetBackendDispatchFunction(packet);
}
 
FORCEINLINE const void* CommandPacket::LoadCommand(const CmdPacket packet)
{
    return reinterpret_cast<char*>(packet) + OFFSET_COMMAND;
}

TRE_NS_END