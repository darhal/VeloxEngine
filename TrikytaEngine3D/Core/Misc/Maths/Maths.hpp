#pragma once

#include <math.h>
#include <float.h>
#include "Common.hpp"
#include "Vec.hpp"
#include "Vec1.hpp"
#include "Vec2.hpp"
#include "Vec3.hpp"
#include "Vec4.hpp"
#include "Vec4f_simd.hpp"
#include "Vec4i_simd.hpp"
#include "Matrix.hpp"
#include "Matrix4x4.hpp"

typedef Vec<1, float, normal> vec1;
typedef Vec<2, float, normal> vec2;
typedef Vec<3, float, normal> vec3;
typedef Vec<4, float, normal> vec4;
typedef Matrix<float, SIMD, 4, 4> mat4;