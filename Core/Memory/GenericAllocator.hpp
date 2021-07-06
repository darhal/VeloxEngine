#pragma once

#include <Core/Misc/Defines/Common.hpp>
#include <Core/Memory/Memory.hpp>

TRE_NS_START

class GenericAllocator
{
public:
    FORCEINLINE constexpr void* AllocateBytes(usize sz, usize al = 1)
    {
        // FIXME : this is a temp hack
        if (al == 1)
            return Utils::AllocateBytes(sz);
        return Utils::AllocateBytes(sz, al);
    }

    FORCEINLINE constexpr void Free(void* ptr) noexcept
    {
        return Utils::FreeMemory(ptr);
    }

private:

};

TRE_NS_END
