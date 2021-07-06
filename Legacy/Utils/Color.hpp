#pragma once

#include <Legacy/Misc/Defines/Common.hpp>

TRE_NS_START

class Color
{
public:
	union { struct { uint8 r, g, b, a; }; struct { uint8 R, G, B, A; }; };

	Color() {}

	Color(uint8 r, uint8 g, uint8 b, uint8 a = 255) :  r(r), g(g), b(b), a(a)
	{}

	Color(int32 r, int32 g, int32 b, int32 a = 255) : r(uint8(r)), g(uint8(g)), b(uint8(b)), a(uint8(a))
	{}

	Color(float r, float g, float b, float a) : r(uint8(r*255.f)), g(uint8(g*255.f)), b(uint8(b*255.f)), a(uint8(a*255.f))
	{}

	/*FORCEINLINE const float* normalize() const
	{
		const float col[4] = { R / 255.0f, G / 255.0f, B / 255.0f, A / 255.0f };
		return col;
	}*/
private:
	
};

TRE_NS_END