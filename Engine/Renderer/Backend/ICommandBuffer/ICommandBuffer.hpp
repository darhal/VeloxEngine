#pragma once

#include "Core/Misc/Defines/Common.hpp"
#include "Renderer/Backend/CommandBucket/CommandBucket.hpp"
#include "Core/Memory/Allocators/LinearAlloc/LinearAllocator.hpp"

TRE_NS_START

class ICommandBuffer
{
public:
	// ICommandBuffer();

	void DispatchCommands() const;

	CommandBucket& CreateBucket();

	CommandBucket& GetCommandBucker(uint32 i);

	uint32 GetBucketsCount() const;
private:
	Vector<CommandBucket> m_Buckets;
};

void ICommandBuffer::DispatchCommands() const
{
	for (const CommandBucket& bucket : m_Buckets) {
		bucket.Flush();
	}
}

CommandBucket& ICommandBuffer::CreateBucket()
{
	return m_Buckets.EmplaceBack();
}

CommandBucket& ICommandBuffer::GetCommandBucker(uint32 i)
{
	return m_Buckets[i];
}

uint32 ICommandBuffer::GetBucketsCount() const
{
	return (uint32)m_Buckets.Size();
}

TRE_NS_END