#include "CommandBucket.hpp"
#include <Renderer/Backend/ResourcesManager/ResourcesManager.hpp>
#include <RenderAPI/Shader/ShaderProgram.hpp>

TRE_NS_START

CommandBucket::CommandBucket() : m_RenderTarget(), m_Projection(), m_Camera(), m_Packets(), m_PacketAllocator(sizeof(CommandPacket), 1)
{
}

CommandBucket::CommandBucket(CommandBucket&& bucket) : 
	m_RenderTarget(std::move(bucket.m_RenderTarget)), 
	m_Projection(bucket.m_Projection),
	m_Camera(bucket.m_Camera),
	m_Packets(std::move(bucket.m_Packets)), 
	m_PacketAllocator(std::move(bucket.m_PacketAllocator))
{
}

CommandBucket& CommandBucket::operator=(CommandBucket&& bucket)
{
	m_RenderTarget = std::move(bucket.m_RenderTarget);
	m_Projection = bucket.m_Projection;
	m_Camera = bucket.m_Camera;
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
	new (buff) CommandPacket(this, key);
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
	ResourcesManager& manager = ResourcesManager::Instance();
	// Apply framer buffer
	const Mat4f projView = m_Projection * m_Camera.GetViewMatrix();

	for (const auto& key_packet_pair : m_Packets) {
		// Decode and apply key
		uint32 shader_id;
		RenderState state = RenderState::FromKey(key_packet_pair.first, &shader_id);
		state.ApplyStates();

		// Set Shader
		ShaderProgram& shader = manager.Get<ShaderProgram>(ResourcesTypes::SHADER, shader_id);
		shader.Bind();
		shader.SetMat4("ProjView", projView);

		key_packet_pair.second->Flush(manager, shader, projView);
		key_packet_pair.second->SwapBuffer();
	}
}

void CommandBucket::End()
{
	Map<BucketKey, CommandPacket*>::CIterator itr = m_Packets.crbegin();
	Map<BucketKey, CommandPacket*>::CIterator end = m_Packets.crend();

	for (itr; itr != end; itr--) {
		if (itr->key < BLEND_ENABLED) { // Check if blending is enabled
			break;
		}

		itr->value->SortCommands();
	}
}

TRE_NS_END