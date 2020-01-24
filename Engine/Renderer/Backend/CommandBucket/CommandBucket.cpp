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

void CommandBucket::OnKeyChangeCallback(ResourcesManager& manager, const BucketKey& key, const Mat4f& proj_view, const Mat4f& proj, const Camera& camera)
{
	uint32 shader_id;
	RenderState state = RenderState::FromKey(key, &shader_id);
	state.ApplyStates();

	// Set Shader
	ShaderProgram& shader = manager.Get<ShaderProgram>(ResourcesTypes::SHADER, shader_id);
	shader.Bind();
	shader.SetMat4("u_ProjView", proj_view);
	shader.SetVec3("u_ViewPosition", camera.Position);
}

void CommandBucket::OnBucketFlushCallback(ResourcesManager& manager, const RenderTarget& rt)
{
	// Apply framer buffer here!
	if (rt.m_FboID != 0) {
		FBO& fbo = manager.Get<FBO>(ResourcesTypes::FBO, rt.m_FboID);
		fbo.Bind();
	}

	// TODO: Maybe set viewport width and height
}


void CommandBucket::Flush() const
{
	ResourcesManager& manager = ResourcesManager::Instance();
	// Call call back function
	if (m_OnBucketFlushCallback)
		m_OnBucketFlushCallback(manager, m_RenderTarget);

	const Mat4f projView = m_Projection * m_Camera.GetViewMatrix();
	const vec3& camera_position = m_Camera.Position;

	for (const auto& key_packet_pair : m_Packets) {
		// Decode the key,  and apply render states and shader
		if (m_OnKeyChangeCallback)
			m_OnKeyChangeCallback(manager, key_packet_pair.first, projView, m_Projection, m_Camera);

		key_packet_pair.second->Flush();
		key_packet_pair.second->SwapBuffer();
	}
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