#pragma once

#include <Core/Misc/Defines/Common.hpp>
#include <Core/Misc/Defines/Debug.hpp>
#include <Core/Memory/Utils/Utils.hpp>

TRE_NS_START

class BaseAllocator
{
public:
	virtual ~BaseAllocator() {};

	virtual void* Allocate(usize sz = 0, usize alignement = 0) = 0;
	virtual void Deallocate(void* ptr) = 0;
};

TRE_NS_END