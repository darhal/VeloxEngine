#pragma once

#include <Renderer/Backend/Common.hpp>
#include <new>

TRE_NS_START

namespace Renderer
{
    namespace Utils
    {
        FORCEINLINE void* AlignedAlloc(size_t size, size_t alignement)
        {
            ASSERTF((alignement & (alignement - 1)), "Alignement (value=%u) isn't power of 2", (uint32)alignement);

            void** place;
            uintptr_t addr = 0;
            void* ptr = ::operator new(alignement + size + sizeof(uintptr_t));

            if (ptr == NULL)
                return NULL;

            addr = ((uintptr_t)ptr + sizeof(void*) + alignement) & ~(alignement - 1);
            place = (void**)addr;
            place[-1] = ptr;

            // void* addr = (void*)::operator new(size, (std::align_val)alignement);
            return (void*)addr;
        }

        FORCEINLINE void AlignedFree(void* ptr)
        {
            if (ptr != NULL) {
                void** p = (void**)ptr;
                ::operator delete(p[-1]);
            }

            //::operator delete ptr;
        }
    }
}

TRE_NS_END
