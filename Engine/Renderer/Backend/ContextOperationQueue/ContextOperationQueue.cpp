#include "ContextOperationQueue.hpp"

TRE_NS_START

ContextOperationQueue::ContextOperationQueue() :
	m_CmdsAllocator(TOTAL_SIZE * MULTIPLIER, true), m_CmdsCount(0), m_BufferMarker(0)
{
	m_Commands = (Cmd*) m_CmdsAllocator.Allocate(DEFAULT_MAX_ELEMENTS * COMMAND_PTR * MULTIPLIER);
}

void ContextOperationQueue::Flush()
{
	if (m_CmdsCount) {
		const uint32 start = m_BufferMarker * DEFAULT_MAX_ELEMENTS;
		const uint32 end = m_BufferMarker * DEFAULT_MAX_ELEMENTS + m_CmdsCount;

		for (uint32 i = start; i < end; i++) {
			Cmd cmd = m_Commands[i];

			// Issue the command.
			const BackendDispatchFunction CommandFunction = Command::LoadBackendDispatchFunction(cmd);
			const void* command = Command::LoadCommand(cmd);
			CommandFunction(command);
		}

		this->SwapBuffer();
	}
}

void ContextOperationQueue::SwapBuffer()
{
	m_CmdsCount = 0;
	m_BufferMarker = !m_BufferMarker;
	m_CmdsAllocator.SetOffset(DEFAULT_MAX_ELEMENTS * COMMAND_PTR * MULTIPLIER + DEFAULT_MAX_ELEMENTS * COMMAND_SIZE * m_BufferMarker);
}

TRE_NS_END