#include "Command.hpp"

TRE_NS_START

void Command::StoreBackendDispatchFunction(Cmd cmd, BackendDispatchFunction dispatchFunction)
{
	*Command::GetBackendDispatchFunction(cmd) = dispatchFunction;
}

BackendDispatchFunction Command::LoadBackendDispatchFunction(Cmd cmd)
{
	return *Command::GetBackendDispatchFunction(cmd);
}

BackendDispatchFunction* Command::GetBackendDispatchFunction(Cmd cmd)
{
	return reinterpret_cast<BackendDispatchFunction*>(reinterpret_cast<char*>(cmd));
}

const void* Command::LoadCommand(Cmd cmd)
{
	return (void*)(reinterpret_cast<uint8*>(cmd) + OFFSET_COMMAND);
}

TRE_NS_END