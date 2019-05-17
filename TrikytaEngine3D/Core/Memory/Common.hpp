#pragma once

#include <Core/Memory/Allocators/LinearAlloc/LinearAllocator.hpp>
#include <Core/Memory/Allocators/StackAlloc/StackAllocator.hpp>
#include <Core/Memory/Allocators/StackAlloc/AlignedStackAllocator.hpp>
#include <Core/Memory/Allocators/PoolAlloc/PoolAllocator.hpp>

namespace TRE {
	typedef LinearAllocator			LinearAlloc;
	typedef StackAllocator			StackAlloc;
	typedef AlignedStackAllocator	AlignStackAlloc;
	typedef PoolAllocator			PoolAlloc;
}
