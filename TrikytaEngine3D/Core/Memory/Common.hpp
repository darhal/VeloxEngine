#pragma once

#include <Core/Memory/LinearAlloc/LinearAllocator.hpp>
#include <Core/Memory/StackAlloc/StackAllocator.hpp>
#include <Core/Memory/StackAlloc/AlignedStackAllocator.hpp>
#include <Core/Memory/PoolAlloc/PoolAllocator.hpp>

namespace TRE {
	typedef LinearAllocator			LinearAlloc;
	typedef StackAllocator			StackAlloc;
	typedef AlignedStackAllocator	AlignStackAlloc;
	typedef PoolAllocator			PoolAlloc;
}
