#ifndef UTILS_HPP
#define UTILS_HPP

#include <Core/Misc/Defines/Common.hpp>

TRE_NS_START

namespace Utils
{
    template<typename T>
    constexpr usize Strlen(const T* str)
    {
        usize size = 0;
        while (*str) {
            str++;
            size++;
        }
        return size;
    }
}

TRE_NS_END

#endif // UTILS_HPP
