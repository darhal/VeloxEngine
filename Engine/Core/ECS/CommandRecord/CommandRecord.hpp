#pragma once

#include <Core/Misc/Defines/Common.hpp>
#include <Core/Memory/Allocators/LinearAlloc/LinearAllocator.hpp>
#include <Core/ECS/Commands/Commands.hpp>

TRE_NS_START

class CommandRecord
{
public:
	CommandRecord();

	template<typename CMD>
	CMD* SubmitCommand();

	template<typename CMD>
	void* CreateCommand();

	void Flush();

	void Clear();
private:
	LinearAllocator m_Allocator;
	void** m_Commands;
	uint32 m_Count;

	CONSTEXPR static uint32 CMD_SIZE = 1024;
	CONSTEXPR static uint32 CMD_PTR = sizeof(void**);
	CONSTEXPR static uint32 CMD_COUNT = 1024;
	CONSTEXPR static uint32 CMD_TOTAL = CMD_COUNT * (CMD_SIZE + CMD_PTR);
	CONSTEXPR static uint32 OFFSET_COMMAND = sizeof(ECSCommands::CommandFunction*);
};

template<typename CMD>
CMD* CommandRecord::SubmitCommand()
{
	ASSERTF(m_Count >= CMD_COUNT, "ECS Command Record buffer is full.");

	void* cmd = CommandRecord::CreateCommand<CMD>();
	m_Commands[m_Count++] = cmd;
	return (CMD*)(void*)(reinterpret_cast<uint8*>(cmd) + OFFSET_COMMAND);
}

template<typename CMD>
void* CommandRecord::CreateCommand()
{
	void* cmd = m_Allocator.Allocate(sizeof(CMD) + sizeof(ECSCommands::CommandFunction));
	*reinterpret_cast<ECSCommands::CommandFunction*>(reinterpret_cast<char*>(cmd)) = CMD::DISPATCH_FUNCTION;
	return cmd;
}


TRE_NS_END