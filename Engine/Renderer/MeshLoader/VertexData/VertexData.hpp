#pragma once

#include <Core/Misc/Defines/Common.hpp>
#include <Core/Misc/Maths/Vec3.hpp>

TRE_NS_START

struct VertexData
{
	VertexData(const vec3& p, const vec3& n, const vec2& t) : pos(p), normal(n), texture(t)
	{}

	vec3 pos, normal;
	vec2 texture;
};

TRE_NS_END