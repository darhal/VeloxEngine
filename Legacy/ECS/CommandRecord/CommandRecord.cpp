#include "CommandRecord.hpp"

TRE_NS_START

CommandRecord::CommandRecord() : 
	m_Allocator(CMD_TOTAL, true),
	m_Commands((void**)m_Allocator.Allocate(CMD_COUNT * CMD_PTR)),
	m_Count(0)
{
}

void CommandRecord::Flush()
{
	if (m_Count) {
		for (uint32 i = 0; i < m_Count; i++) {
			void* cmd = m_Commands[i];
			const ECSCommands::CommandFunction callback = *reinterpret_cast<ECSCommands::CommandFunction*>((uint8*)cmd);
			const void* cmd_data = (void*)((uint8*)(cmd) + OFFSET_COMMAND);
			callback(cmd_data);
		}

		this->Clear();
	}
}

void CommandRecord::Clear()
{
	m_Count = 0;
	m_Allocator.SetOffset(CMD_COUNT * CMD_PTR);
}

TRE_NS_END