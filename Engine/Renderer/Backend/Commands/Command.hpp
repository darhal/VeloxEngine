#pragma once

#include <Core/Misc/Defines/Common.hpp>
#include <Renderer/Common/Common.hpp>
#include <Core/Memory/Allocators/BaseAlloc/BaseAllocator.hpp>

TRE_NS_START

typedef void* Cmd;

namespace Command
{
	/*CallBackFunc | Command */
	template<typename T>
	Cmd CreateCommand(BaseAllocator& alloc);

	template<typename T>
	T* GetCommand(Cmd cmd);

	const void* LoadCommand(Cmd cmd);

	BackendDispatchFunction* GetBackendDispatchFunction(Cmd cmd);

	void StoreBackendDispatchFunction(Cmd cmd, BackendDispatchFunction dispatchFunction);

	BackendDispatchFunction LoadBackendDispatchFunction(Cmd cmd);

	CONSTEXPR static usize OFFSET_COMMAND = sizeof(BackendDispatchFunction);
};

template<typename T>
FORCEINLINE Cmd Command::CreateCommand(BaseAllocator& alloc)
{
	const uint32 total_size = OFFSET_COMMAND + sizeof(T);
	Cmd cmd = alloc.Allocate(total_size);
	return cmd;
}

template <typename T>
T* Command::GetCommand(Cmd cmd)
{
	return (T*)(reinterpret_cast<uint8*>(cmd) + OFFSET_COMMAND);
}

TRE_NS_END