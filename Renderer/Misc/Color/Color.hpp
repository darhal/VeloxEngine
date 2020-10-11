#pragma once

#include <Renderer/Common.hpp>

TRE_NS_START

namespace Renderer
{
    struct Color
    {
        union
        {
            struct { uint8 r, g, b, a; };
            uint32 hex;
        };

        Color(uint8 r, uint8 g, uint8 b, uint8 a = 255) :
            r(r), g(g), b(b), a(a)
        {
        }

        Color() : hex(0) {}

        operator uint32()
        {
            return hex;
        }
    };
}

TRE_NS_END