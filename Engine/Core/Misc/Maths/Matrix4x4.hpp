#pragma once

#if __cplusplus <= 201103L
	#include <initializer_list>
#endif

#include <iostream>
#include "Vec.hpp"
#include <Core/Misc/Defines/PlatformSIMDInclude.hpp>

#undef near
#undef far

TRE_NS_START

template<typename TYPE, proc_type proc, uint8 R, uint8 C>
class Matrix;

template<>
struct Matrix<float, SIMD, 4, 4>
{
public:
	typedef Matrix class_type;

	union {
		__m128 rows[4];
		float m[4][4];
		float data[16];
		Vec<4, float, SIMD> v0;
		Vec<4, float, SIMD> v1; 
		Vec<4, float, SIMD> v2; 
		Vec<4, float, SIMD> v3; 
	};

	FORCEINLINE Matrix();

	template<typename TYPE>
	FORCEINLINE Matrix(TYPE);

	FORCEINLINE Matrix(bool);

	template<proc_type proc>
	FORCEINLINE Matrix(const Vec<4, float, proc>& vec0, const Vec<4, float, proc>& vec1, const Vec<4, float, proc>& vec2, const Vec<4, float, proc>& vec3);

	FORCEINLINE Matrix(
		float x0, float y0, float z0, float w0,
		float x1, float y1, float z1, float w1,
		float x2, float y2, float z2, float w2,
		float x3, float y3, float z3, float w3
	);

	//Destructor
	FORCEINLINE ~Matrix() {};

	FORCEINLINE float determinant();

	FORCEINLINE class_type& operator=(const class_type&);

	template<typename T, proc_type Q>
	FORCEINLINE class_type& scale(const Vec<3, T, Q>&);
	template<typename T, proc_type Q>
	FORCEINLINE class_type& translate(const Vec<3, T, Q>&);
	template<typename T, proc_type Q>
	FORCEINLINE class_type& rotate(const Vec<3, T, Q>&, float);
	FORCEINLINE class_type& rotateZ(float);
	FORCEINLINE class_type& rotateY(float);
	FORCEINLINE class_type& rotateX(float);

	FORCEINLINE class_type transpose();
	FORCEINLINE class_type transpose_inplace();
	FORCEINLINE class_type transform_inverse_noscale();
	FORCEINLINE class_type transform_inverse();
	FORCEINLINE class_type inverse();
	FORCEINLINE const float* ptr() const;

	static FORCEINLINE class_type frustum(float left, float right, float bottom, float top, float near, float far);
	static FORCEINLINE class_type perspective(float fovy, float aspect, float near, float far);
	static FORCEINLINE class_type ortho(float left, float right, float bottom, float top, float near, float far);

	template<proc_type proc>
	static FORCEINLINE class_type look_at(const Vec<3, float, proc>& eye, const Vec<3, float, proc>& center, const Vec<3, float, proc>& up);

	template<proc_type proc>
	static FORCEINLINE Vec<3, float, proc> unproject(const Vec<3, float, proc>& vec, const class_type& view, const class_type& proj, const float viewport[]);

	template<proc_type proc>
	static FORCEINLINE Vec<3, float, proc> project(const Vec<4, float, proc>& vec, const class_type& view, const class_type& proj, const float viewport[]);

	FORCEINLINE class_type& operator *= (const class_type&);
	FORCEINLINE class_type& operator *= (const float);
	FORCEINLINE class_type& operator += (const class_type&);
	FORCEINLINE class_type& operator -= (const class_type&);

	FORCEINLINE friend class_type operator + (const class_type&, const class_type&);
	FORCEINLINE friend class_type operator - (const class_type&, const class_type&);
	FORCEINLINE friend class_type operator + (const class_type&);
	FORCEINLINE friend class_type operator - (const class_type&);

	template<typename T>
	friend FORCEINLINE class_type operator*(const class_type& mat, T scalar);
	template<typename T>
	friend FORCEINLINE class_type operator*(T scalar, const class_type& mat);
	friend FORCEINLINE class_type operator*(const class_type& mat1, const class_type& mat2);
	friend FORCEINLINE Vec<4, float, SIMD> operator*(const Vec<4, float, SIMD>&, const class_type&);

	FORCEINLINE void print();
#if __cplusplus <= 201103L
	FORCEINLINE Matrix(const std::initializer_list<std::initializer_list<float>>&);
#endif

};

typedef Matrix<float, SIMD, 4, 4> mat4;
typedef Matrix<float, SIMD, 4, 4> Mat4f;

FORCEINLINE Matrix<float, SIMD, 4, 4>::Matrix(
	float x0, float y0, float z0, float w0,
	float x1, float y1, float z1, float w1,
	float x2, float y2, float z2, float w2,
	float x3, float y3, float z3, float w3)
{
	rows[0] = _mm_setr_ps(x0, y0, z0, w0);
	rows[1] = _mm_setr_ps(x1, y1, z1, w1);
	rows[2] = _mm_setr_ps(x2, y2, z2, w2);
	rows[3] = _mm_setr_ps(x3, y3, z3, w3);
}

template<proc_type proc>
FORCEINLINE Matrix<float, SIMD, 4, 4>::Matrix(const Vec<4, float, proc>& vec0, const Vec<4, float, proc>& vec1, const Vec<4, float, proc>& vec2, const Vec<4, float, proc>& vec3)
{
	rows[0] = _mm_setr_ps(vec0.x, vec0.y, vec0.z, vec0.w);
	rows[1] = _mm_setr_ps(vec1.x, vec1.y, vec1.z, vec1.w);
	rows[2] = _mm_setr_ps(vec2.x, vec2.y, vec2.z, vec2.w);
	rows[3] = _mm_setr_ps(vec3.x, vec3.y, vec3.z, vec3.w);
}

FORCEINLINE Matrix<float, SIMD, 4, 4>::Matrix()
{
	rows[0] = _mm_setr_ps(1.f, 0.f, 0.f, 0.f);
	rows[1] = _mm_setr_ps(0.f, 1.f, 0.f, 0.f);
	rows[2] = _mm_setr_ps(0.f, 0.f, 1.f, 0.f);
	rows[3] = _mm_setr_ps(0.f, 0.f, 0.f, 1.f);
}

template<typename TYPE>
FORCEINLINE Matrix<float, SIMD, 4, 4>::Matrix(TYPE s)
{
	float v = static_cast<float>(s);
	for (int i = 0; i < 4; i++) {
		rows[i] = _mm_set1_ps(v);
	}
}

FORCEINLINE Matrix<float, SIMD, 4, 4>::Matrix(bool)
{
}

FORCEINLINE Matrix<float, SIMD, 4, 4>& Matrix<float, SIMD, 4, 4>::operator=(const Matrix<float, SIMD, 4, 4>& v)
{
	for (int i = 0; i < 4; i++) {
		rows[i] = v.rows[i];
	}
	return *this;
}

FORCEINLINE Matrix<float, SIMD, 4, 4> operator*(const Matrix<float, SIMD, 4, 4>& mat1, const Matrix<float, SIMD, 4, 4>& mat2)
{
	Matrix<float, SIMD, 4, 4> res(false);
	for (int i = 0; i < 4; i++) {
		__m128 brod1 = _mm_set1_ps(_m128(mat1.rows[i]).m128_f32[0]);
		__m128 brod2 = _mm_set1_ps(_m128(mat1.rows[i]).m128_f32[1]);
		__m128 brod3 = _mm_set1_ps(_m128(mat1.rows[i]).m128_f32[2]);
		__m128 brod4 = _mm_set1_ps(_m128(mat1.rows[i]).m128_f32[3]);
		res.rows[i] = 
			_mm_add_ps(
				_mm_add_ps(
					_mm_mul_ps(mat2.rows[0], brod1),
					_mm_mul_ps(mat2.rows[1], brod2)),
				_mm_add_ps(
					_mm_mul_ps(mat2.rows[2], brod3),
					_mm_mul_ps(mat2.rows[3], brod4))
			);
	}
	return res;
}

FORCEINLINE Vec<4, float, SIMD> operator*(const Vec<4, float, SIMD>& v, const Matrix<float, SIMD, 4, 4>& mat)
{
	__m128 result;
	result = _mm_mul_ps(_mm_replicate_x_ps(v), mat.rows[0]);
	result = _mm_madd_ps(_mm_replicate_y_ps(v), mat.rows[1], result);
	result = _mm_madd_ps(_mm_replicate_z_ps(v), mat.rows[2], result);
	result = _mm_madd_ps(_mm_replicate_w_ps(v), mat.rows[3], result);
	return Vec<4, float, SIMD>(result);
}

FORCEINLINE void Matrix<float, SIMD, 4, 4>::print()
{
	for (uint8 i = 0; i < 4; i++) {
		_m128 x = rows[i];
		for (uint8 j = 0; j < 4; j++) {
			std::cout << x.m128_f32[j] << ", ";
		}
		std::cout << std::endl;
	}
}

FORCEINLINE float Matrix<float, SIMD, 4, 4>::determinant() {
	__m128 Va, Vb, Vc;
	__m128 r1, r2, r3, t1, t2, sum;
	Vec<4, float, SIMD> Det;

	// First, Let's calculate the first four minterms of the first line
	t1 = rows[3]; t2 = _mm_ror_ps(rows[2], 1);
	Vc = _mm_mul_ps(t2, _mm_ror_ps(t1, 0));                   // V3'�V4
	Va = _mm_mul_ps(t2, _mm_ror_ps(t1, 2));                   // V3'�V4"
	Vb = _mm_mul_ps(t2, _mm_ror_ps(t1, 3));                   // V3'�V4^

	r1 = _mm_sub_ps(_mm_ror_ps(Va, 1), _mm_ror_ps(Vc, 2));     // V3"�V4^ - V3^�V4"
	r2 = _mm_sub_ps(_mm_ror_ps(Vb, 2), _mm_ror_ps(Vb, 0));     // V3^�V4' - V3'�V4^
	r3 = _mm_sub_ps(_mm_ror_ps(Va, 0), _mm_ror_ps(Vc, 1));     // V3'�V4" - V3"�V4'

	Va = _mm_ror_ps(rows[1], 1);     sum = _mm_mul_ps(Va, r1);
	Vb = _mm_ror_ps(Va, 1);      sum = _mm_add_ps(sum, _mm_mul_ps(Vb, r2));
	Vc = _mm_ror_ps(Vb, 1);      sum = _mm_add_ps(sum, _mm_mul_ps(Vc, r3));

	// Now we can calculate the determinant:
	Det = _mm_mul_ps(sum, rows[0]);
	Det = _mm_add_ps(Det, _mm_movehl_ps(Det, Det));
	Det = _mm_sub_ss(Det, _mm_shuffle_ps(Det, Det, 1));
	return Det[0];
}

#if __cplusplus <= 201103L
	FORCEINLINE Matrix<float, SIMD, 4, 4>::Matrix(const std::initializer_list<std::initializer_list<float>>& list)
	{
		uint8 i = 0, j;
		for (const std::initializer_list<float>& elm : list) {
			if (i >= 4) { break; }
			j = 0;
			float data[4];
			for (const float v : elm) {
				if (j >= 4) { break; }
				data[j] = v;
				j++;
			}
			rows[i] = _mm_loadu_ps(data);
			i++;
		}
	}
#endif

template<typename T>
FORCEINLINE Matrix<float, SIMD, 4, 4> operator*(const Matrix<float, SIMD, 4, 4>& mat, T scalar)
{
	__m128 s = _mm_set1_ps(static_cast<float>(scalar));
	Matrix<float, SIMD, 4, 4> mat_res(false);
	for (uint8 i = 0; i < 4; i++) {
		mat.rows[i] = _mm_mul_ps(mat.rows[i], s);
	}
	return mat_res;
}

template<typename T>
FORCEINLINE Matrix<float, SIMD, 4, 4> operator*(T scalar, const Matrix<float, SIMD, 4, 4>& mat)
{
	__m128 s = _mm_set1_ps(static_cast<float>(scalar));
	Matrix<float, SIMD, 4, 4> mat_res(false);
	for (uint8 i = 0; i < 4; i++) {
		mat.rows[i] = _mm_mul_ps(s, mat.rows[i]);
	}
	return mat_res;
}

template<typename T, proc_type Q>
FORCEINLINE Matrix<float, SIMD, 4, 4>& Matrix<float, SIMD, 4, 4>::translate(const Vec<3, T, Q>& v)
{
	return *this = *this * Matrix<float, SIMD, 4, 4>(
		1, 0, 0, v.x,
		0, 1, 0, v.y,
		0, 0, 1, v.z,
		0, 0, 0, 1
	);
}

template<typename T, proc_type Q>
FORCEINLINE Matrix<float, SIMD, 4, 4>& Matrix<float, SIMD, 4, 4>::scale(const Vec<3, T, Q>& v)
{
	return *this = *this * Matrix<float, SIMD, 4, 4>(
		v.x, 0, 0, 0,
		0, v.y, 0, 0,
		0, 0, v.z, 0,
		0, 0, 0, 1
	);
}

FORCEINLINE Matrix<float, SIMD, 4, 4>& Matrix<float, SIMD, 4, 4>::rotateX(float ang)
{
	return *this = *this * Matrix<float, SIMD, 4, 4>(
		1, 0, 0, 0,
		0, cos(ang), -sin(ang), 0,
		0, sin(ang), cos(ang), 0,
		0, 0, 0, 1
	);
}

FORCEINLINE Matrix<float, SIMD, 4, 4>& Matrix<float, SIMD, 4, 4>::rotateY(float ang)
{
	return *this = *this * Matrix<float, SIMD, 4, 4>(
		cos(ang), 0, sin(ang), 0,
		0, 1, 0, 0,
		-sin(ang), 0, cos(ang), 0,
		0, 0, 0, 1
	);
}

FORCEINLINE Matrix<float, SIMD, 4, 4>& Matrix<float, SIMD, 4, 4>::rotateZ(float ang)
{
	return *this = *this * Matrix<float, SIMD, 4, 4>(
		cos(ang), -sin(ang), 0, 0,
		sin(ang), cos(ang), 0, 0,
		0, 0, 1, 0,
		0, 0, 0, 1
	);
}

template<typename T, proc_type Q>
FORCEINLINE Matrix<float, SIMD, 4, 4>& Matrix<float, SIMD, 4, 4>::rotate(const Vec<3, T, Q>& axis, float ang)
{
	float s = sin(ang);
	float c = cos(ang);
	float t = 1 - c;
	Vec<3, T, Q> a = axis.unit();

	return *this = *this * Matrix<float, SIMD, 4, 4>(
		a.x * a.x * t + c, a.x * a.y * t - a.z * s, a.x * a.z * t + a.y * s, 0,
		a.y * a.x * t + a.z * s, a.y * a.y * t + c, a.y * a.z * t - a.x * s, 0,
		a.z * a.x * t - a.y * s, a.z * a.y * t + a.x * s, a.z * a.z * t + c, 0,
		0, 0, 0, 1
	);
}

// Requires this matrix to be transform matrix, NoScale version requires this matrix be of scale 1
FORCEINLINE Matrix<float, SIMD, 4, 4> Matrix<float, SIMD, 4, 4>::transform_inverse_noscale()
{
	Matrix<float, SIMD, 4, 4> r(false);

	// transpose 3x3, we know m03 = m13 = m23 = 0
	__m128 t0 = VecShuffle_0101(rows[0], rows[1]); // 00, 01, 10, 11
	__m128 t1 = VecShuffle_2323(rows[0], rows[1]); // 02, 03, 12, 13
	r.rows[0] = VecShuffle(t0, rows[2], 0, 2, 0, 3); // 00, 10, 20, 23(=0)
	r.rows[1] = VecShuffle(t0, rows[2], 1, 3, 1, 3); // 01, 11, 21, 23(=0)
	r.rows[2] = VecShuffle(t1, rows[2], 0, 2, 2, 3); // 02, 12, 22, 23(=0)

	// last line
	r.rows[3] = _mm_mul_ps(r.rows[0], VecSwizzle1(rows[3], 0));
	r.rows[3] = _mm_add_ps(r.rows[3], _mm_mul_ps(r.rows[1], VecSwizzle1(rows[3], 1)));
	r.rows[3] = _mm_add_ps(r.rows[3], _mm_mul_ps(r.rows[2], VecSwizzle1(rows[3], 2)));
	r.rows[3] = _mm_sub_ps(_mm_setr_ps(0.f, 0.f, 0.f, 1.f), r.rows[3]);

	return r;
}

#define SMALL_NUMBER		(1.e-8f)

// Requires this matrix to be transform matrix
FORCEINLINE Matrix<float, SIMD, 4, 4> Matrix<float, SIMD, 4, 4>::transform_inverse()
{
	Matrix<float, SIMD, 4, 4> r(false);

	// transpose 3x3, we know m03 = m13 = m23 = 0
	__m128 t0 = VecShuffle_0101(rows[0], rows[1]); // 00, 01, 10, 11
	__m128 t1 = VecShuffle_2323(rows[0], rows[1]); // 02, 03, 12, 13
	r.rows[0] = VecShuffle(t0, rows[2], 0, 2, 0, 3); // 00, 10, 20, 23(=0)
	r.rows[1] = VecShuffle(t0, rows[2], 1, 3, 1, 3); // 01, 11, 21, 23(=0)
	r.rows[2] = VecShuffle(t1, rows[2], 0, 2, 2, 3); // 02, 12, 22, 23(=0)

	// (SizeSqr(rows[0]), SizeSqr(rows[1]), SizeSqr(rows[2]), 0)
	__m128 sizeSqr;
	sizeSqr = _mm_mul_ps(r.rows[0], r.rows[0]);
	sizeSqr = _mm_add_ps(sizeSqr, _mm_mul_ps(r.rows[1], r.rows[1]));
	sizeSqr = _mm_add_ps(sizeSqr, _mm_mul_ps(r.rows[2], r.rows[2]));

	// optional test to avoid divide by 0
	__m128 one = _mm_set1_ps(1.f);
	// for each component, if(sizeSqr < SMALL_NUMBER) sizeSqr = 1;
#if SIMD_SUPPORTED_LEVEL >= 5
	__m128 rSizeSqr = _mm_blendv_ps(
		_mm_div_ps(one, sizeSqr),
		one,
		_mm_cmplt_ps(sizeSqr, _mm_set1_ps(SMALL_NUMBER))
	);
#else
	__m128 rSizeSqr = _mm_or_ps(
		_mm_and_ps(_mm_div_ps(one, sizeSqr), _mm_cmplt_ps(sizeSqr, _mm_set1_ps(SMALL_NUMBER))), _mm_andnot_ps(_mm_cmplt_ps(sizeSqr, _mm_set1_ps(SMALL_NUMBER)), one));
#endif

	r.rows[0] = _mm_mul_ps(r.rows[0], rSizeSqr);
	r.rows[1] = _mm_mul_ps(r.rows[1], rSizeSqr);
	r.rows[2] = _mm_mul_ps(r.rows[2], rSizeSqr);

	// last line
	r.rows[3] = _mm_mul_ps(r.rows[0], VecSwizzle1(rows[3], 0));
	r.rows[3] = _mm_add_ps(r.rows[3], _mm_mul_ps(r.rows[1], VecSwizzle1(rows[3], 1)));
	r.rows[3] = _mm_add_ps(r.rows[3], _mm_mul_ps(r.rows[2], VecSwizzle1(rows[3], 2)));
	r.rows[3] = _mm_sub_ps(_mm_setr_ps(0.f, 0.f, 0.f, 1.f), r.rows[3]);

	return r;
}

// Inverse function is the same no matter column major or row major
// this version treats it as row major
FORCEINLINE Matrix<float, SIMD, 4, 4> Matrix<float, SIMD, 4, 4>::inverse()
{
	// The inverse is calculated using "Divide and Conquer" technique. The 
	// original matrix is divide into four 2x2 sub-matrices. Since each 
	// register holds four matrix element, the smaller matrices are 
	// represented as a registers. Hence we get a better locality of the 
	// calculations.

	Vec<4, float, SIMD> A = _mm_movelh_ps(rows[0], rows[1]),    // the four sub-matrices 
		B = _mm_movehl_ps(rows[1], rows[0]),
		C = _mm_movelh_ps(rows[2], rows[3]),
		D = _mm_movehl_ps(rows[3], rows[2]);
	Vec<4, float, SIMD> iA, iB, iC, iD, DC, AB;	// partial inverse of the sub-matrices
	__m128 dA, dB, dC, dD;                 // determinant of the sub-matrices
	__m128 det, d, d1, d2;
	Vec<4, float, SIMD> rd;

	//  AB = A# * B
	AB = _mm_mul_ps(_mm_shuffle_ps(A, A, 0x0F), B);
	AB -= (Vec<4, float, SIMD>)_mm_mul_ps(_mm_shuffle_ps(A, A, 0xA5), _mm_shuffle_ps(B, B, 0x4E));
	//  DC = D# * C
	DC = _mm_mul_ps(_mm_shuffle_ps(D, D, 0x0F), C);
	DC -= (Vec<4, float, SIMD>)_mm_mul_ps(_mm_shuffle_ps(D, D, 0xA5), _mm_shuffle_ps(C, C, 0x4E));

	//  dA = |A|
	dA = _mm_mul_ps(_mm_shuffle_ps(A, A, 0x5F), A);
	dA = _mm_sub_ss(dA, _mm_movehl_ps(dA, dA));
	//  dB = |B|
	dB = _mm_mul_ps(_mm_shuffle_ps(B, B, 0x5F), B);
	dB = _mm_sub_ss(dB, _mm_movehl_ps(dB, dB));

	//  dC = |C|
	dC = _mm_mul_ps(_mm_shuffle_ps(C, C, 0x5F), C);
	dC = _mm_sub_ss(dC, _mm_movehl_ps(dC, dC));
	//  dD = |D|
	dD = _mm_mul_ps(_mm_shuffle_ps(D, D, 0x5F), D);
	dD = _mm_sub_ss(dD, _mm_movehl_ps(dD, dD));

	//  d = trace(AB*DC) = trace(A#*B*D#*C)
	d = _mm_mul_ps(_mm_shuffle_ps(DC, DC, 0xD8), AB);

	//  iD = C*A#*B
	iD = _mm_mul_ps(_mm_shuffle_ps(C, C, 0xA0), _mm_movelh_ps(AB, AB));
	iD += (Vec<4, float, SIMD>)_mm_mul_ps(_mm_shuffle_ps(C, C, 0xF5), _mm_movehl_ps(AB, AB));
	//  iA = B*D#*C
	iA = _mm_mul_ps(_mm_shuffle_ps(B, B, 0xA0), _mm_movelh_ps(DC, DC));
	iA += (Vec<4, float, SIMD>)_mm_mul_ps(_mm_shuffle_ps(B, B, 0xF5), _mm_movehl_ps(DC, DC));

	//  d = trace(AB*DC) = trace(A#*B*D#*C) [continue]
	d  = _mm_add_ps(d, _mm_movehl_ps(d, d));
	d  = _mm_add_ss(d, _mm_shuffle_ps(d, d, 1));
	d1 = _mm_mul_ss(dA, dD);
	d2 = _mm_mul_ss(dB, dC);

	//  iD = D*|A| - C*A#*B
	iD = D * (Vec<4, float, SIMD>)_mm_shuffle_ps(dA, dA, 0) - iD;

	//  iA = A*|D| - B*D#*C;
	iA = A * (Vec<4, float, SIMD>)_mm_shuffle_ps(dD, dD, 0) - iA;

	//  det = |A|*|D| + |B|*|C| - trace(A#*B*D#*C)
	det = _mm_sub_ss(_mm_add_ss(d1, d2), d);
	rd = (__m128)(_mm_div_ss(_mm_set_ss(1.0f), det));
#ifdef ZERO_SINGULAR
	rd = _mm_and_ps(_mm_cmpneq_ss(det, _mm_setzero_ps()), rd);
#endif

	//  iB = D * (A#B)# = D*B#*A
	iB = _mm_mul_ps(D, _mm_shuffle_ps(AB, AB, 0x33));
	iB -= (Vec<4, float, SIMD>)_mm_mul_ps(_mm_shuffle_ps(D, D, 0xB1), _mm_shuffle_ps(AB, AB, 0x66));
	//  iC = A * (D#C)# = A*C#*D
	iC = _mm_mul_ps(A, _mm_shuffle_ps(DC, DC, 0x33));
	iC -= (Vec<4, float, SIMD>)_mm_mul_ps(_mm_shuffle_ps(A, A, 0xB1), _mm_shuffle_ps(DC, DC, 0x66));

	rd = _mm_shuffle_ps(rd, rd, 0);
	rd ^= Sign_PNNP;

	//  iB = C*|B| - D*B#*A
	iB = C * (Vec<4, float, SIMD>)_mm_shuffle_ps(dB, dB, 0) - iB;

	//  iC = B*|C| - A*C#*D;
	iC = B * (Vec<4, float, SIMD>)_mm_shuffle_ps(dC, dC, 0) - iC;

	//  iX = iX / det
	iA *= rd;
	iB *= rd;
	iC *= rd;
	iD *= rd;

	Matrix<float, SIMD, 4, 4> res(false);
	res.rows[0] = _mm_shuffle_ps(iA, iB, 0x77);
	res.rows[1] = _mm_shuffle_ps(iA, iB, 0x22);
	res.rows[2] = _mm_shuffle_ps(iC, iD, 0x77);
	res.rows[3] = _mm_shuffle_ps(iC, iD, 0x22);
	return res;
}


FORCEINLINE const float* Matrix<float, SIMD, 4, 4>::ptr() const
{
	return &(this->m[0][0]);
}

FORCEINLINE Matrix<float, SIMD, 4, 4> Matrix<float, SIMD, 4, 4>::transpose() {
	Matrix<float, SIMD, 4, 4> res(false);
	__m128  xmm0 = _mm_unpacklo_ps(rows[0], rows[1]),
		xmm1 = _mm_unpacklo_ps(rows[2], rows[3]),
		xmm2 = _mm_unpackhi_ps(rows[0], rows[1]),
		xmm3 = _mm_unpackhi_ps(rows[2], rows[3]);

	res.rows[0] = _mm_movelh_ps(xmm0, xmm1);
	res.rows[1] = _mm_movehl_ps(xmm1, xmm0);
	res.rows[2] = _mm_movelh_ps(xmm2, xmm3);
	res.rows[3] = _mm_movehl_ps(xmm3, xmm2);
	return res;
}


FORCEINLINE Matrix<float, SIMD, 4, 4> Matrix<float, SIMD, 4, 4>::transpose_inplace() {
	__m128  xmm0 = _mm_unpacklo_ps(rows[0], rows[1]),
		xmm1 = _mm_unpacklo_ps(rows[2], rows[3]),
		xmm2 = _mm_unpackhi_ps(rows[0], rows[1]),
		xmm3 = _mm_unpackhi_ps(rows[2], rows[3]);

	rows[0] = _mm_movelh_ps(xmm0, xmm1);
	rows[1] = _mm_movehl_ps(xmm1, xmm0);
	rows[2] = _mm_movelh_ps(xmm2, xmm3);
	rows[3] = _mm_movehl_ps(xmm3, xmm2);
	return *this;
}


FORCEINLINE Matrix<float, SIMD, 4, 4>& Matrix<float, SIMD, 4, 4>::operator*=(const Matrix<float, SIMD, 4, 4>& A)
{
	for (uint8 i = 0; i < 4; i++) {
		rows[i] = _mm_mul_ps(rows[i], A.rows[i]);
	}
	return *this;
}

FORCEINLINE Matrix<float, SIMD, 4, 4>& Matrix<float, SIMD, 4, 4>::operator*=(const float S)
{
	__m128 s = _mm_set_ps1(S);
	for (uint8 i = 0; i < 4; i++) {
		rows[i] = _mm_mul_ps(rows[i], s);
	}
	return *this;
}

FORCEINLINE Matrix<float, SIMD, 4, 4>& Matrix<float, SIMD, 4, 4>::operator+=(const Matrix<float, SIMD, 4, 4>& A)
{
	for (uint8 i = 0; i < 4; i++) {
		rows[i] = _mm_add_ps(rows[i], A.rows[i]);
	}
	return *this;
}

FORCEINLINE Matrix<float, SIMD, 4, 4>& Matrix<float, SIMD, 4, 4>::operator-=(const Matrix<float, SIMD, 4, 4>& A)
{
	for (uint8 i = 0; i < 4; i++) {
		rows[i] = _mm_sub_ps(rows[i], A.rows[i]);
	}
	return *this;
}

FORCEINLINE Matrix<float, SIMD, 4, 4> operator+ (const Matrix<float, SIMD, 4, 4>& A, const Matrix<float, SIMD, 4, 4>& B)
{
	Matrix<float, SIMD, 4, 4> res(false);
	for (uint8 i = 0; i < 4; i++) {
		res.rows[i] = _mm_add_ps(A.rows[i], B.rows[i]);
	}
	return res;
}

FORCEINLINE Matrix<float, SIMD, 4, 4> operator-(const Matrix<float, SIMD, 4, 4>& A, const Matrix<float, SIMD, 4, 4>& B)
{
	Matrix<float, SIMD, 4, 4> res(false);
	for (uint8 i = 0; i < 4; i++) {
		res.rows[i] = _mm_sub_ps(A.rows[i], B.rows[i]);
	}
	return res;
}

FORCEINLINE Matrix<float, SIMD, 4, 4> operator+(const Matrix<float, SIMD, 4, 4>& A)
{
	return A;
}

FORCEINLINE Matrix<float, SIMD, 4, 4> operator-(const Matrix<float, SIMD, 4, 4>& A)
{
	Matrix<float, SIMD, 4, 4> res(false);
	__m128 masksign = _MASKSIGNs_;
	for (uint8 i = 0; i < 4; i++) {
		res.rows[i] = _mm_xor_ps(masksign, A.rows[i]);
	}
	return res;
}

FORCEINLINE Matrix<float, SIMD, 4, 4> Matrix<float, SIMD, 4, 4>::frustum(float left, float right, float bottom, float top, float near, float far)
{
	Matrix<float, SIMD, 4, 4> res(false);

	//Row Major
	res.rows[0] = _mm_setr_ps(near * 2.0f / (right - left), 0.f, (right + left) / (right - left), 0.f);
	res.rows[1] = _mm_setr_ps(0.f, near * 2.0f / (top - bottom), (top + bottom) / (top - bottom), 0.f);
	res.rows[2] = _mm_setr_ps(0.f, 0.f, (-far - near) / (far - near), -2.0f * far * near / (far - near));
	res.rows[3] = _mm_setr_ps(0.f, 0.f, -1.f, 0.f);

	/*
	//Colum Major
	res.rows[0] = _mm_setr_ps(near * 2.0f / (right - left), 0.f, 0.f, 0.f);
	res.rows[1] = _mm_setr_ps(0.f, near * 2.0f / (top - bottom), 0.f, 0.f);
	res.rows[2] = _mm_setr_ps((right + left) / (right - left), (top + bottom) / (top - bottom), (-far - near) / (far - near), -1.f);
	res.rows[3] = _mm_setr_ps(0.f, 0.f, -2.0f * far * near / (far - near), 0.f);
	*/
	return res;
}

FORCEINLINE Matrix<float, SIMD, 4, 4> Matrix<float, SIMD, 4, 4>::perspective(float fovy, float aspect, float near, float far)
{
	float top = near * tan(fovy / 2.0f);
	float right = top * aspect;
	return frustum(-right, right, -top, top, near, far);
}

FORCEINLINE Matrix<float, SIMD, 4, 4> Matrix<float, SIMD, 4, 4>::ortho(float left, float right, float bottom, float top, float near, float far)
{
	Matrix<float, SIMD, 4, 4> res(false);

	//Row Major
	res.rows[0] = _mm_setr_ps(2 / (right - left), 0.f, 0.f, -(left + right) / (right - left));
	res.rows[1] = _mm_setr_ps(0.f, 2 / (top - bottom), 0.f, -(top + bottom) / (top - bottom));
	res.rows[2] = _mm_setr_ps(0.f, 0.f, -2 / (far - near), -(far + near) / (far - near));
	res.rows[3] = _mm_setr_ps(0.f, 0.f, 0.f, 1.f);
	
	//Colum Major
	/*res.rows[0] = _mm_setr_ps(2 / (right - left), 0.f, 0.f, 0.f);
	res.rows[1] = _mm_setr_ps(0.f, 2 / (top - bottom), 0.f, 0.f);
	res.rows[2] = _mm_setr_ps(0.f, 0.f, -2 / (far - near), 0.f);
	res.rows[3] = _mm_setr_ps(-(left + right) / (right - left), -(top + bottom) / (top - bottom), -(far + near) / (far - near), 1.f);*/
	return res;
}

template<proc_type proc>
FORCEINLINE Matrix<float, SIMD, 4, 4> Matrix<float, SIMD, 4, 4>::look_at(const Vec<3, float, proc>& eye, const Vec<3, float, proc>& center, const Vec<3, float, proc>& up)
{
	Matrix<float, SIMD, 4, 4> res(false);

	Vec<3, float, proc> Z = (eye - center).unit();

	Vec<3, float, proc> X = Vec<3, float, proc>(
		up.y * Z.z - up.z * Z.y,
		up.z * Z.x - up.x * Z.z,
		up.x * Z.y - up.y * Z.x
	).unit();

	Vec<3, float, proc> Y = Vec<3, float, proc>(
		Z.y * X.z - Z.z * X.y,
		Z.z * X.x - Z.x * X.z,
		Z.x * X.y - Z.y * X.x
	).unit();

	//Row Major
	res.rows[0] = _mm_setr_ps(X.x, X.y, X.z, -X.dot_product(eye));
	res.rows[1] = _mm_setr_ps(Y.x, Y.y, Y.z, -Y.dot_product(eye));
	res.rows[2] = _mm_setr_ps(Z.x, Z.y, Z.z, -Z.dot_product(eye));
	res.rows[3] = _mm_setr_ps(0.f, 0.f, 0.f, 1.f);

	//Colum Major
	/*res.rows[0] = _mm_setr_ps(X.x, Y.x, Z.x, 0.f);
	res.rows[1] = _mm_setr_ps(X.y, Y.y, Z.y, 0.f);
	res.rows[2] = _mm_setr_ps(X.z, Y.z, Z.z, 0.f);
	res.rows[3] = _mm_setr_ps(-X.dot_product(eye), -Y.dot_product(eye), -Z.dot_product(eye), 1.f);*/
	return res;
}

template<proc_type proc>
FORCEINLINE Vec<3, float, proc> unproject(const Vec<3, float, proc>& vec, const Matrix<float, SIMD, 4, 4>& view, const Matrix<float, SIMD, 4, 4>& proj, const float viewport[])
{
	Matrix<float, SIMD, 4, 4> inv = (proj * view).inverse();
	Vec<3, float, proc> v(
		(vec.x - viewport[0]) * 2.0f / viewport[2] - 1.0f,
		(vec.y - viewport[1]) * 2.0f / viewport[3] - 1.0f,
		2.0f * vec.z - 1.0f
	);
	Vec<3, float, proc> res = inv * v;
	float w = inv.m[0][3] * v.x + inv.m[1][7] * v.y + inv.m[2][11] * v.z + inv.m[3][15];
	return res / w;
}

template<proc_type proc>
FORCEINLINE Vec<3, float, proc> project(const Vec<4, float, proc>& vec, const Matrix<float, SIMD, 4, 4>& view, const Matrix<float, SIMD, 4, 4>& proj, const float viewport[])
{
	Matrix<float, SIMD, 4, 4> trans = proj * view;
	Vec<4, float, SIMD> v = trans * vec;

	float w = trans.m[0][3] * vec.x + trans.m[1][7] * vec.y + trans.m[2][11] * vec.z + trans.m[3][15];
	v = v / w;

	return Vec<3, float, proc>(
		viewport[0] + viewport[2] * (v.x + 1.0f) / 2.0f,
		viewport[1] + viewport[3] * (v.y + 1.0f) / 2.0f,
		(v.z + 1.0f) / 2.0f
	);
}

TRE_NS_END