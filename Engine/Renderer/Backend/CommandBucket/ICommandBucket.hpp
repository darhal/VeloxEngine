#pragma once

#include <Core/Misc/Defines/Common.hpp>
#include <Core/Misc/Maths/Matrix4x4.hpp>
#include <Core/Memory/Allocators/PoolAlloc/MultiPoolAllocator.hpp>

#include <Renderer/Backend/Keys/BucketKey.hpp>
#include <Renderer/Backend/CommandBucket/CommandPacket/CommandPacket.hpp>

TRE_NS_START

class ICommandBucket
{
public:
	virtual void* SubmitCommand(usize cmd_size, BucketKey key) = 0;

	virtual void Flush() const = 0;

	virtual void Finalize() = 0;
};

TRE_NS_END