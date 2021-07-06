#pragma once

#include <Core/Misc/Defines/Common.hpp>
#include <Core/Memory/Memory.hpp>

TRE_NS_START

template<usize SIZE = 4096>
class LocalAllocator
{
public:
    FORCEINLINE constexpr void* AllocateBytes(usize sz, usize al = 1)
    {
        // FIXME : this doesn't take into account alignement
        usize remaining = SIZE - m_Reserved;

        if (sz < remaining) {
            m_Reserved += sz;
            m_LatestSize = sz;
            return m_Buffer;
        }

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
        uint8* oldPtr = m_Buffer - m_LatestSize;
        usize remaining = SIZE - m_Reserved;
        usize addition = sz - m_LatestSize;

        if (oldPtr == ptr && addition < remaining) {
            m_LatestSize = sz;
            return ptr;
        }

        return Utils::AllocateBytes(sz);
    }

    template<typename T>
    FORCEINLINE constexpr T* Reallocate(T* ptr, usize count)
    {
        void* data = this->ReallocateBytes(ptr, sizeof(T) * count);
        return static_cast<T*>(data);
    }

    FORCEINLINE constexpr void FreeMemory(void* ptr) noexcept
    {
        if (ptr < m_Buffer && ptr >= m_Buffer + SIZE)
            Utils::FreeMemory(ptr);
    }

private:
    uint8 m_Buffer[SIZE];
    usize m_Reserved = 0;
    usize m_LatestSize = 0;
};

TRE_NS_END
