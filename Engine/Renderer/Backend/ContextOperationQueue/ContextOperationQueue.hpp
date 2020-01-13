#pragma once

#include <Core/Misc/Defines/Common.hpp>
#include <Renderer/Common/Common.hpp>
#include <Renderer/Backend/Commands/Command.hpp>
#include <Core/Memory/Allocators/LinearAlloc/LinearAllocator.hpp>

TRE_NS_START

class ContextOperationQueue
{
public:
	ContextOperationQueue();

	template<typename U>
	U* SubmitCommand();

	void Flush();

	void SwapBuffer();
private:
	LinearAllocator m_CmdsAllocator;
	Cmd* m_Commands;
	uint32 m_CmdsCount;
	uint8 m_BufferMarker;

	CONSTEXPR static uint32 DEFAULT_MAX_ELEMENTS = 2048;
	CONSTEXPR static uint32 COMMAND_SIZE = 1024;
	CONSTEXPR static uint32 COMMAND_PTR = sizeof(Cmd);
	CONSTEXPR static uint32 TOTAL_SIZE = DEFAULT_MAX_ELEMENTS * (COMMAND_SIZE + COMMAND_PTR);
	CONSTEXPR static uint32 MULTIPLIER = 2;
};


template<typename U>
U* ContextOperationQueue::SubmitCommand()
{
	Cmd cmd = Command::CreateCommand<U>(m_CmdsAllocator);
	Command::StoreBackendDispatchFunction(cmd, U::DISPATCH_FUNCTION);
	m_Commands[m_BufferMarker * DEFAULT_MAX_ELEMENTS + m_CmdsCount++] = cmd;
	return Command::GetCommand<U>(cmd);
}

TRE_NS_END

