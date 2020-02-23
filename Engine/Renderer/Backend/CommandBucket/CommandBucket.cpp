#include "CommandBucket.hpp"
#include <Renderer/Backend/ResourcesManager/ResourcesManager.hpp>
#include <RenderAPI/Shader/ShaderProgram.hpp>
#include <RenderAPI/FrameBuffer/FBO.hpp>

TRE_NS_START

CommandBucket::CommandBucket() : 
	m_RenderTarget(), m_OnKeyChangeCallback(OnKeyChangeCallback), m_OnBucketFlushCallback(OnBucketFlushCallback),
	m_Projection(), m_Camera(), m_Packets(), m_PacketAllocator(sizeof(CommandPacket), 1)
{
}

CommandBucket::CommandBucket(CommandBucket&& bucket) : 
	m_RenderTarget(std::move(bucket.m_RenderTarget)), 
	m_OnKeyChangeCallback(bucket.m_OnKeyChangeCallback),
	m_OnBucketFlushCallback(bucket.m_OnBucketFlushCallback),
	m_Projection(bucket.m_Projection),
	m_Camera(bucket.m_Camera),
	m_Packets(std::move(bucket.m_Packets)), 
	m_PacketAllocator(std::move(bucket.m_PacketAllocator))
{
	memcpy(m_ExtraBuffer, bucket.m_ExtraBuffer, sizeof(uint8) * EXTRA_BUFFER_SPACE);
}

CommandBucket& CommandBucket::operator=(CommandBucket&& bucket)
{
	m_RenderTarget = std::move(bucket.m_RenderTarget);
	m_OnKeyChangeCallback = bucket.m_OnKeyChangeCallback;
	m_OnBucketFlushCallback = bucket.m_OnBucketFlushCallback;
	m_Projection = bucket.m_Projection;
	m_Camera = bucket.m_Camera;
	m_Packets = std::move(bucket.m_Packets);
	m_PacketAllocator = std::move(bucket.m_PacketAllocator);
	memcpy(m_ExtraBuffer, bucket.m_ExtraBuffer, sizeof(uint8) * EXTRA_BUFFER_SPACE);
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

ShaderProgram& CommandBucket::OnKeyChangeCallback(ResourcesManager& manager, const BucketKey& key, const Mat4f& proj_view, const Mat4f& proj, const Camera& camera, const uint8* extra_data)
{
	uint32 shader_id;
	RenderState state = RenderState::FromKey(key, &shader_id);
	state.ApplyStates();

	// Set Shader
	ShaderProgram& shader = manager.Get<ShaderProgram>(shader_id);
	shader.Bind();
	shader.SetMat4("u_ProjView", proj_view);
	shader.SetVec3("u_ViewPosition", camera.Position);
	return shader;
}

FBO& CommandBucket::OnBucketFlushCallback(ResourcesManager& manager, const RenderTarget& rt, const uint8* extra_data)
{
	// Apply framer buffer here!
	//if (rt.m_FboID != 0) {
	FBO& fbo = manager.Get<FBO>(rt.m_FboID);
	fbo.Bind();
	Clear(Buffer::COLOR | Buffer::DEPTH);
	//}

	// TODO: Maybe set viewport width and height
	return fbo;
}

void CommandBucket::Flush() const
{
	ResourcesManager& manager = ResourcesManager::Instance();
	// Call call back function
	if (m_OnBucketFlushCallback)
		m_OnBucketFlushCallback(manager, m_RenderTarget, m_ExtraBuffer);

	const Mat4f projView = m_Projection * m_Camera.GetViewMatrix();
	const vec3& camera_position = m_Camera.Position;

	printf("************* [BUCKET] *************\n");
	for (const auto& key_packet_pair : m_Packets) {
		// Decode the key,  and apply render states and shader
		if (m_OnKeyChangeCallback)
			m_OnKeyChangeCallback(manager, key_packet_pair.first, projView, m_Projection, m_Camera, m_ExtraBuffer);

		key_packet_pair.second->Flush();
		key_packet_pair.second->SwapBuffer();
	}
	printf("************* [END OF BUCKET] *************\n");
}

void CommandBucket::Finalize()
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