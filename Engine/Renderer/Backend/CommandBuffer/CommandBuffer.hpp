#pragma once

#include "Core/Misc/Defines/Common.hpp"
#include "Renderer/Backend/CommandBucket/CommandBucket.hpp"
#include "Core/Memory/Allocators/LinearAlloc/LinearAllocator.hpp"

TRE_NS_START

class CommandBuffer
{
public:
	CommandBuffer();

	void DispatchCommands() const;

	CommandBucket& CreateBucket();

	CommandBucket& GetCommandBucker(uint32 i);

	uint32 GetBucketsCount() const;
private:
	Vector<CommandBucket> m_Buckets;
};


TRE_NS_END