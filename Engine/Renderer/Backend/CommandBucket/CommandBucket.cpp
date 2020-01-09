#include "CommandBucket.hpp"

TRE_NS_START

CommandBucket::CommandBucket() : m_RenderTarget(), m_ProjViewMat(), m_Packets(), m_PacketAllocator(sizeof(CommandPacket), 1)
{
}

CommandBucket::CommandBucket(CommandBucket&& bucket) : 
	m_RenderTarget(std::move(bucket.m_RenderTarget)), 
	m_ProjViewMat(bucket.m_ProjViewMat),
	m_Packets(std::move(bucket.m_Packets)), 
	m_PacketAllocator(std::move(bucket.m_PacketAllocator))
{
}

CommandBucket& CommandBucket::operator=(CommandBucket&& bucket)
{
	m_RenderTarget = std::move(bucket.m_RenderTarget);
	m_ProjViewMat = bucket.m_ProjViewMat;
	m_Packets = std::move(bucket.m_Packets);
	m_PacketAllocator = std::move(bucket.m_PacketAllocator);
	return *this;
}

CommandPacket* CommandBucket::GetCommandPacket(const BucketKey& key) const
{
	CommandPacket** res = NULL;

	if ((res = m_Packets.Get(key)) != NULL)
		return *res;

	return NULL;
}

CommandPacket& CommandBucket::CreateCommandPacket(const BucketKey& key)
{
	CommandPacket* buff = m_PacketAllocator.Allocate<CommandPacket>();
	new (buff) CommandPacket(key);
	m_Packets.Emplace(key, buff);
	return *buff;
}

CommandPacket& CommandBucket::GetOrCreateCommandPacket(const BucketKey& key)
{
	CommandPacket* res = GetCommandPacket(key);

	if (!res)
		return CreateCommandPacket(key);

	return *res;
}

void CommandBucket::Flush() const
{
	// Apply framer buffer and set view proj mat
	printf("Flush bucket\n");

	for (const auto& key_packet_pair : m_Packets) {
		// Decode and apply key
		uint32 shader_id;
		RenderState state = RenderState::FromKey(key_packet_pair.first, &shader_id);
		state.ApplyStates();
		key_packet_pair.second->Flush();
	}
}

void CommandBucket::End()
{
	Map<BucketKey, CommandPacket*>::CIterator itr = m_Packets.cend();
	Map<BucketKey, CommandPacket*>::CIterator end = m_Packets.cbegin();
	for (itr; itr != end; itr--) {
		if (itr->key < 9) { // Check if blending is enabled
			break;
		}

		printf("SORT KEY : %d\n", itr->key);
		itr->value->SortCommands();
	}
}

TRE_NS_END