#pragma once

#include <math.h>
#include <float.h>
#include "Vec.hpp"

TRE_NS_START

template<uint8 N, typename TYPE, proc_type Q>
struct Vec;

template<typename TYPE>
struct Vec<1, TYPE, normal>;
template<typename TYPE>
struct Vec<2, TYPE, normal>;
template<typename TYPE>
struct Vec<3, TYPE, normal>;
template<typename TYPE>
struct Vec<4, TYPE, normal>;
template<typename TYPE, proc_type proc, uint8 R, uint8 C>
class Matrix;

template<typename T>
using Vec2 = TRE::Vec<2, T, normal>;
template<typename T>
using Vec3 = TRE::Vec<3, T, normal>;
template<typename T>
using Vec4 = TRE::Vec<4, T, normal>;

typedef Vec<1, float, normal> vec1;
typedef Vec<2, float, normal> vec2;
typedef Vec<3, float, normal> vec3;
typedef Vec<4, float, normal> vec4;
typedef Matrix<float, SIMD, 4, 4> mat4;

TRE_NS_END

#include "Utils.hpp"