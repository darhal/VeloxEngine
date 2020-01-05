#include "CommandPacket.hpp"
#include <algorithm>

TRE_NS_START

CommandPacket::CommandPacket(const BucketKey& key) : 
	m_CmdsAllocator(TOTAL_SIZE * MULTIPLIER, true),
	m_Key(key), m_CmdsCount(0), m_BufferMarker(0)
{
	m_Commands = (Pair<uint64, Cmd>*) m_CmdsAllocator.Allocate(DEFAULT_MAX_ELEMENTS * COMMAND_PTR * MULTIPLIER);
}

void CommandPacket::Flush()
{
	const uint32 start = m_BufferMarker * DEFAULT_MAX_ELEMENTS;
	const uint32 end = m_BufferMarker * DEFAULT_MAX_ELEMENTS + m_CmdsCount;

	for (uint32 i = start; i < end; i++) {
		const uint64& internal_key = m_Commands[i].first;
		Cmd cmd = m_Commands[i].second;
		const BackendDispatchFunction CommandFunction = Command::LoadBackendDispatchFunction(cmd);
		const void* command = Command::LoadCommand(cmd);
		printf("[INTERNAL_KEY:%d]:", internal_key);
		CommandFunction(command);
	}
}

void CommandPacket::SetKey(const BucketKey& key)
{
	m_Key = key;
}

void CommandPacket::SwapBuffer()
{
	m_CmdsAllocator.SetOffset(DEFAULT_MAX_ELEMENTS * COMMAND_PTR * MULTIPLIER + DEFAULT_MAX_ELEMENTS * COMMAND_SIZE * m_BufferMarker);
	m_BufferMarker = !m_BufferMarker;	
}

void CommandPacket::SortCommands()
{
	printf("Sortying packt with key : %d\n", m_Key);

	const uint32 start = m_BufferMarker * DEFAULT_MAX_ELEMENTS;
	const uint32 end = m_BufferMarker * DEFAULT_MAX_ELEMENTS + m_CmdsCount;

	std::qsort(m_Commands + start, end, sizeof(Pair<BucketKey, Cmd>), [](const void* a, const void* b) {
		const Pair<BucketKey, Cmd>& arg1 = *static_cast<const Pair<BucketKey, Cmd>*>(a);
		const Pair<BucketKey, Cmd>& arg2 = *static_cast<const Pair<BucketKey, Cmd>*>(b);

		if (arg1.first < arg2.first) return -1;
		if (arg1.first > arg2.first) return 1;
		return 0;
	});
}

TRE_NS_END