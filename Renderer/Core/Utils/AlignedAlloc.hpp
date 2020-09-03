#pragma once

#include <Renderer/Common.hpp>

TRE_NS_START

namespace Renderer
{
    namespace Utils
    {
        FORCEINLINE void* AlignedAlloc(size_t size, size_t alignement)
        {
            void** place;
            uintptr_t addr = 0;
            void* ptr = ::operator new(alignement + size + sizeof(uintptr_t));

            if (ptr == NULL)
                return NULL;

            addr = ((uintptr_t)ptr + sizeof(uintptr_t) + alignement) & ~(alignement - 1);
            place = (void**)addr;
            place[-1] = ptr;

            return (void*)addr;
        }

        FORCEINLINE void AlignedFree(void* ptr)
        {
            if (ptr != NULL) {
                void** p = (void**)ptr;
                free(p[-1]);
            }
        }
    }
}

TRE_NS_END