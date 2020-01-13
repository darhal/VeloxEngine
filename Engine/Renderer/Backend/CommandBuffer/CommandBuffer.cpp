#include "CommandBuffer.hpp"

TRE_NS_START

CommandBuffer::CommandBuffer()
{
}

void CommandBuffer::DispatchCommands() const
{
	for (const CommandBucket& bucket : m_Buckets) {
		bucket.Flush();
	}
}

CommandBucket& CommandBuffer::CreateBucket()
{
	return m_Buckets.EmplaceBack();
}

CommandBucket& CommandBuffer::GetCommandBucker(uint32 i)
{
	return m_Buckets[i];
}

uint32 CommandBuffer::GetBucketsCount() const
{
	return (uint32)m_Buckets.Size();
}

TRE_NS_END