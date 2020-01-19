#pragma once

#include <Core/Misc/Defines/Common.hpp>
#include <Core/Misc/Maths/Matrix4x4.hpp>
#include <Core/DataStructure/HashMap/Map.hpp>
#include <Core/Memory/Allocators/PoolAlloc/MultiPoolAllocator.hpp>

#include <Renderer/Backend/Keys/BucketKey.hpp>
#include <Renderer/Backend/RenderTarget/RenderTarget.hpp>
#include <Renderer/Backend/CommandBucket/CommandPacket/CommandPacket.hpp>
#include <Renderer/Backend/RenderState/RenderState.hpp>
#include <Renderer/Backend/Camera/Camera.hpp>

TRE_NS_START

class CommandBucket
{
public:
	CommandBucket();

	CommandBucket(const CommandBucket&) = delete;

	CommandBucket& operator=(const CommandBucket&) = delete;

	CommandBucket(CommandBucket&& bucket);

	CommandBucket& operator=(CommandBucket&& bucket);

	template<typename U>
	CommandPacket* SubmitCommand(U** cmd_out, const BucketKey& key, const uint64& internal_key = 0);

	template<typename U>
	U* SubmitCommand(const BucketKey& key, const uint64& internal_key = 0);

	template<typename U>
	U* SubmitCommand(const RenderState& render_state = RenderState(), uint32 shader_id = 0, const uint64& internal_key = 0);

	CommandPacket* GetCommandPacket(const BucketKey& key) const;

	CommandPacket& CreateCommandPacket(const BucketKey& key);

	CommandPacket& GetOrCreateCommandPacket(const BucketKey& key);

	const Camera& GetCamera() const { return m_Camera; }

	Camera& GetCamera() { return m_Camera; }

	Mat4f& GetProjectionMatrix() { return m_Projection; }

	const Mat4f& GetProjectionMatrix() const { return m_Projection; }

	RenderTarget& GetRenderTarget() { return m_RenderTarget; }

	const RenderTarget& GetRenderTarget() const { return m_RenderTarget; }

	void Flush() const;

	void Finalize();
private:
	RenderTarget m_RenderTarget;
	Mat4f m_Projection;
	Camera m_Camera;
	Map<BucketKey, CommandPacket*> m_Packets;
	MultiPoolAllocator m_PacketAllocator;

	CONSTEXPR static BucketKey BLEND_ENABLED = (BucketKey(1) << 63);
};

template<typename U>
CommandPacket* CommandBucket::SubmitCommand(U** cmd_out, const BucketKey& key, const uint64& internal_key)
{
	CommandPacket& packet = GetOrCreateCommandPacket(key);
	*cmd_out = packet.SubmitCommand<U>(internal_key);
	return &packet;
}

template<typename U>
U* CommandBucket::SubmitCommand(const BucketKey& key, const uint64& internal_key)
{
	CommandPacket& packet = GetOrCreateCommandPacket(key);
	return packet.SubmitCommand<U>(internal_key);
}

template<typename U>
U* CommandBucket::SubmitCommand(const RenderState& render_state, uint32 shader_id, const uint64& internal_key)
{
	CommandPacket& packet = GetOrCreateCommandPacket(render_state.ToKey(shader_id));
	return packet.SubmitCommand<U>(internal_key);
}

TRE_NS_END