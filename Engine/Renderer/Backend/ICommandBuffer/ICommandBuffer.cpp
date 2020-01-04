#include "ICommandBuffer.hpp"

TRE_NS_START

/*ICommandBuffer::ICommandBuffer() : m_Buckets()
{}*/

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
	return m_Buckets.Size();
}

TRE_NS_END