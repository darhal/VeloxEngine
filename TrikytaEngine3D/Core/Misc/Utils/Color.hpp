#pragma once

#include <Core/Misc/Defines/Common.hpp>

TRE_NS_START

class Color
{
public:
	union { struct { uint8 r, g, b, a; }; struct { uint8 R, G, B, A; }; };

	Color() {}
	Color(uint8 r, uint8 g, uint8 b, uint8 a = 255) :  r(r), g(g), b(b), a(a)
	{}
	Color(float r, float g, float b, float a) : r(uint8(r*255.f)), g(uint8(g*255.f)), b(uint8(b*255.f)), a(uint8(a*255.f))
	{}
private:
	
};

TRE_NS_END