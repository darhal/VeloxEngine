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

    template<typename T>
    FORCEINLINE constexpr T* Allocate(usize count)
    {
        void* data = this->AllocateBytes(sizeof(T) * count);
        return static_cast<T*>(data);
    }

    FORCEINLINE constexpr void* ReallocateBytes(void* ptr, usize sz, usize al = 1)
    {
        return this->AllocateBytes(sz, al);
    }

    template<typename T>
    FORCEINLINE constexpr T* Reallocate(T* ptr, usize count)
    {
        void* data = this->AllocateBytes(sizeof(T) * count);
        return static_cast<T*>(data);
    }

    FORCEINLINE constexpr void FreeMemory(void* ptr) noexcept
    {
        return Utils::FreeMemory(ptr);
    }

private:

};

TRE_NS_END
