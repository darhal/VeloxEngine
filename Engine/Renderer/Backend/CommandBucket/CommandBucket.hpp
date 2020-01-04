#pragma once

#include <Core/Misc/Defines/Common.hpp>
#include <Core/Misc/Maths/Matrix4x4.hpp>
#include <Core/DataStructure/HashMap/Map.hpp>
#include <Core/Memory/Allocators/PoolAlloc/MultiPoolAllocator.hpp>

#include <Renderer/Backend/Commands/Key/Key.hpp>
#include <Renderer/Backend/RenderTarget/RenderTarget.hpp>
#include <Renderer/Backend/CommandBucket/CommandPacket/CommandPacket.hpp>

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
	U* SubmitCommand(const Key& key, const uint64& internal_key = 0);

	CommandPacket* GetCommandPacket(const Key& key) const;

	CommandPacket& CreateCommandPacket(const Key& key);

	CommandPacket& GetOrCreateCommandPacket(const Key& key);

	void Flush() const;

	void End();
private:
	RenderTarget m_RenderTarget;
	Mat4f m_ProjViewMat;
	Map<Key, CommandPacket*> m_Packets;
	MultiPoolAllocator m_PacketAllocator;
};

template<typename U>
U* CommandBucket::SubmitCommand(const Key& key, const uint64& internal_key)
{
	CommandPacket& packet = GetOrCreateCommandPacket(key);
	return packet.SubmitCommand<U>(internal_key);
}

TRE_NS_END