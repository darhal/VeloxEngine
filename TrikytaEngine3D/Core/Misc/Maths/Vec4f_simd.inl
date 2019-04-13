#include "Vec4f_simd.hpp"

TRE_NS_START

Vec<4, float, SIMD>::Vec(float f) 
{
	xmm = _mm_set1_ps(f);
}

Vec<4, float, SIMD>::Vec(float x, float y, float z, float w) 
{
	xmm = _mm_setr_ps(x, y, z, w);
}

Vec<4, float, SIMD>::Vec(const __m128&  x) 
{
	xmm = x;
}

FORCEINLINE Vec<4, float, SIMD>& Vec<4, float, SIMD>::operator=(const __m128& x)
{
	xmm = x;
	return *this;
}

FORCEINLINE Vec<4, float, SIMD>::operator __m128() const
{
	return xmm;
}

FORCEINLINE Vec<4, float, SIMD>& Vec<4, float, SIMD>::load(float const* p)
{
	xmm = _mm_loadu_ps(p);
	return *this;
}

FORCEINLINE Vec<4, float, SIMD>& Vec<4, float, SIMD>::load_a(float const * p)
{
	xmm = _mm_load_ps(p);
	return *this;
}

FORCEINLINE void Vec<4, float, SIMD>::store(float* p) const
{
	_mm_storeu_ps(p, xmm);
}

FORCEINLINE void Vec<4, float, SIMD>::store_a(float* p) const
{
	_mm_store_ps(p, xmm);
}

FORCEINLINE Vec<4, float, SIMD>& Vec<4, float, SIMD>::load_partial(int n, float const* p)
{
	__m128 t1, t2;
	switch (n) {
	case 1:
		xmm = _mm_load_ss(p); break;
	case 2:
		xmm = _mm_castpd_ps(_mm_load_sd((double const*)p)); break;
	case 3:
		t1 = _mm_castpd_ps(_mm_load_sd((double const*)p));
		t2 = _mm_load_ss(p + 2);
		xmm = _mm_movelh_ps(t1, t2); break;
	case 4:
		load(p); break;
	default:
		xmm = _mm_setzero_ps();
	}
	return *this;
}

FORCEINLINE void Vec<4, float, SIMD>::store_partial(int n, float * p) const
{
	__m128 t1;
	switch (n) {
	case 1:
		_mm_store_ss(p, xmm); break;
	case 2:
		_mm_store_sd((double*)p, _mm_castps_pd(xmm)); break;
	case 3:
		_mm_store_sd((double*)p, _mm_castps_pd(xmm));
		t1 = _mm_movehl_ps(xmm, xmm);
		_mm_store_ss(p + 2, t1); break;
	case 4:
		store(p); break;
	default:;
	}
}

FORCEINLINE Vec<4, float, SIMD>& Vec<4, float, SIMD>::cutoff(int n)
{
	if (uint32(n) >= 4) return *this;
	static const union {
		int32 i[8];
		float f[8];
	} mask = { {1,-1,-1,-1,0,0,0,0} };
	xmm = _mm_and_ps(xmm, Vec().load(mask.f + 4 - n));
	return *this;
}

static FORCEINLINE __m128 selectf(const __m128& m, const __m128& b, const __m128& xmm)
{
	return _mm_add_ps(_mm_and_ps(m, b), _mm_andnot_ps(m, xmm));
}

FORCEINLINE const Vec<4, float, SIMD>& Vec<4, float, SIMD>::insert(uint32 index, float value)
{
#if SIMD_SUPPORTED_LEVEL >= 5   // SSE4.1 supported
	switch (index & 3) {
	case 0:
		xmm = _mm_insert_ps(xmm, _mm_set_ss(value), 0 << 4);  break;
	case 1:
		xmm = _mm_insert_ps(xmm, _mm_set_ss(value), 1 << 4);  break;
	case 2:
		xmm = _mm_insert_ps(xmm, _mm_set_ss(value), 2 << 4);  break;
	default:
		xmm = _mm_insert_ps(xmm, _mm_set_ss(value), 3 << 4);  break;
	}
#else
	static const int32 maskl[8] = { 0,0,0,0,-1,0,0,0 };
	__m128 broad = _mm_set1_ps(value);  // broadcast value into all elements
	__m128 mask = _mm_loadu_ps((float const*)(maskl + 4 - (index & 3))); // mask with FFFFFFFF at index position
	xmm = selectf(mask, broad, xmm);
#endif
	return *this;
};


FORCEINLINE float Vec<4, float, SIMD>::extract(uint32 index) const
{
	float x[4];
	store(x);
	return x[index & 3];
}

FORCEINLINE float Vec<4, float, SIMD>::operator[](uint32 index) const
{
	return extract(index);
}


static FORCEINLINE Vec<4, float, SIMD> operator+(const Vec<4, float, SIMD>& a, const Vec<4, float, SIMD>& b) {
	return _mm_add_ps(a, b);
}

static FORCEINLINE Vec<4, float, SIMD> operator+(const Vec<4, float, SIMD>& a, float b) {
	return a + Vec<4, float, SIMD>(b);
}

static FORCEINLINE Vec<4, float, SIMD> operator+(float a, const Vec<4, float, SIMD>& b) {
	return Vec<4, float, SIMD>(a) + b;
}

static FORCEINLINE Vec<4, float, SIMD>& operator+=(Vec<4, float, SIMD>& a, const Vec<4, float, SIMD>& b) {
	a = a + b;
	return a;
}

static FORCEINLINE Vec<4, float, SIMD> operator++(Vec<4, float, SIMD>& a, int) {
	Vec<4, float, SIMD> a0 = a;
	a = a + 1.0f;
	return a0;
}

static FORCEINLINE Vec<4, float, SIMD>& operator++(Vec<4, float, SIMD>& a) {
	a = a + 1.0f;
	return a;
}

static FORCEINLINE Vec<4, float, SIMD> operator-(const Vec<4, float, SIMD>& a,const Vec<4, float, SIMD>& b) {
	return _mm_sub_ps(a, b);
}

static FORCEINLINE Vec<4, float, SIMD> operator-(const Vec<4, float, SIMD>& a, float b) {
	return a - Vec<4, float, SIMD>(b);
}

static FORCEINLINE Vec<4, float, SIMD> operator-(float a, const Vec<4, float, SIMD>& b) {
	return Vec<4, float, SIMD>(a) - b;
}

static FORCEINLINE Vec<4, float, SIMD> operator-(const Vec<4, float, SIMD>& a) {
	return _mm_xor_ps(a, _mm_castsi128_ps(_mm_set1_epi32(0x80000000)));
}

static FORCEINLINE Vec<4, float, SIMD>& operator-=(Vec<4, float, SIMD>& a, const Vec<4, float, SIMD>& b) {
	a = a - b;
	return a;
}

static FORCEINLINE Vec<4, float, SIMD> operator--(Vec<4, float, SIMD>& a, int) {
	Vec<4, float, SIMD> a0 = a;
	a = a - 1.0f;
	return a0;
}

static FORCEINLINE Vec<4, float, SIMD>& operator--(Vec<4, float, SIMD>& a) {
	a = a - 1.0f;
	return a;
}

static FORCEINLINE Vec<4, float, SIMD> operator*(const Vec<4, float, SIMD>& a, const Vec<4, float, SIMD>& b) {
	return _mm_mul_ps(a, b);
}

static FORCEINLINE Vec<4, float, SIMD> operator*(const Vec<4, float, SIMD>& a, float b) {
	return a * Vec<4, float, SIMD>(b);
}

static FORCEINLINE Vec<4, float, SIMD> operator*(float a, const Vec<4, float, SIMD>& b) {
	return Vec<4, float, SIMD>(a) * b;
}

static FORCEINLINE Vec<4, float, SIMD>& operator*=(Vec<4, float, SIMD>& a, const Vec<4, float, SIMD>& b) {
	a = a * b;
	return a;
}

static FORCEINLINE Vec<4, float, SIMD> operator/(const Vec<4, float, SIMD>& a, const Vec<4, float, SIMD>& b) {
	return _mm_div_ps(a, b);
}


static FORCEINLINE Vec<4, float, SIMD> operator/(const Vec<4, float, SIMD>& a, float b) {
	return a / Vec<4, float, SIMD>(b);
}


static FORCEINLINE Vec<4, float, SIMD> operator/(float a, const Vec<4, float, SIMD>& b) {
	return Vec<4, float, SIMD>(a) / b;
}


static FORCEINLINE Vec<4, float, SIMD>& operator/=(Vec<4, float, SIMD>& a, const Vec<4, float, SIMD>& b) {
	a = a / b;
	return a;
}

static FORCEINLINE Vec<4, float, SIMD> operator&(const Vec<4, float, SIMD>& a, const Vec<4, float, SIMD>& b) {
	return _mm_and_ps(a, b);
}

static FORCEINLINE Vec<4, float, SIMD>& operator&=(Vec<4, float, SIMD>& a, const Vec<4, float, SIMD>& b) {
	a = a & b;
	return a;
}



static FORCEINLINE Vec<4, float, SIMD> operator|(const Vec<4, float, SIMD>& a, const Vec<4, float, SIMD>& b) {
	return _mm_or_ps(a, b);
}


static FORCEINLINE Vec<4, float, SIMD>& operator|=(Vec<4, float, SIMD>& a, const Vec<4, float, SIMD>& b) {
	a = a | b;
	return a;
}


static FORCEINLINE Vec<4, float, SIMD> operator^(const Vec<4, float, SIMD>& a, const Vec<4, float, SIMD>& b) {
	return _mm_xor_ps(a, b);
}

static FORCEINLINE Vec<4, float, SIMD>& operator^=(Vec<4, float, SIMD>& a, const Vec<4, float, SIMD>& b) {
	a = a ^ b;
	return a;
}

static FORCEINLINE bool operator== (const Vec<4, float, SIMD>& a, const Vec<4, float, SIMD>& b) {
	_m128 bres = _mm_cmpeq_ps(a, b);
	return unsigned(bres.m128_i32[0]) == 0xffffffff && unsigned(bres.m128_i32[1]) == 0xffffffff && unsigned(bres.m128_i32[2]) == 0xffffffff && unsigned(bres.m128_i32[3]) == 0xffffffff;
}


static FORCEINLINE bool operator!=(const Vec<4, float, SIMD>& a, const Vec<4, float, SIMD>& b) {
	_m128 bres = _mm_cmpneq_ps(a, b);
	return unsigned(bres.m128_i32[0]) == 0xffffffff && unsigned(bres.m128_i32[1]) == 0xffffffff && unsigned(bres.m128_i32[2]) == 0xffffffff && unsigned(bres.m128_i32[3]) == 0xffffffff;
}

static FORCEINLINE bool operator<(const Vec<4, float, SIMD>& a, const Vec<4, float, SIMD>& b) {
	_m128 bres = _mm_cmplt_ps(a, b);
	return unsigned(bres.m128_i32[0]) == 0xffffffff && unsigned(bres.m128_i32[1]) == 0xffffffff && unsigned(bres.m128_i32[2]) == 0xffffffff && unsigned(bres.m128_i32[3]) == 0xffffffff;
}


static FORCEINLINE bool operator<=(const Vec<4, float, SIMD>& a, const Vec<4, float, SIMD>& b) {
	_m128 bres = _mm_cmple_ps(a, b);
	return  unsigned(bres.m128_i32[0]) == 0xffffffff && unsigned(bres.m128_i32[1]) == 0xffffffff && unsigned(bres.m128_i32[2]) == 0xffffffff && unsigned(bres.m128_i32[3]) == 0xffffffff;
}

static FORCEINLINE bool operator>(const Vec<4, float, SIMD>& a, const Vec<4, float, SIMD>& b) {
	return (b < a);
}

static FORCEINLINE bool operator>=(const Vec<4, float, SIMD>& a, const Vec<4, float, SIMD>& b) {
	return (b <= a);
}


FORCEINLINE float Vec<4, float, SIMD>::dot_product(const class_type& v) const
{
	const Vec<4, float, SIMD> res = (*this) * v;
	return res.x + res.y + res.z + res.w;
}

FORCEINLINE float Vec<4, float, SIMD>::angle(const class_type& v) const
{
	return (float)acos(dot_product(v) / length() / v.length());
}

FORCEINLINE float Vec<4, float, SIMD>::lengthSqr() const
{
	return this->dot_product(*this); // return length squared!
}

FORCEINLINE float Vec<4, float, SIMD>::length() const
{
	return (float)sqrt(lengthSqr());
}

FORCEINLINE float Vec<4, float, SIMD>::distance(const class_type& v) const
{
	return (*this - v).length();
}

FORCEINLINE const Vec<4, float, SIMD> Vec<4, float, SIMD>::unit() const
{
	return *this / length();
}

FORCEINLINE const Vec<4, float, SIMD> Vec<4, float, SIMD>::projection(const class_type& v) const
{
	return (dot_product(v) / lengthSqr()) * (*this);
}

FORCEINLINE const float Vec<4, float, SIMD>::scalar_projection(const class_type& v) const
{
	return (dot_product(v) / length());
}

FORCEINLINE const Vec<4, float, SIMD> Vec<4, float, SIMD>::cross_product(const class_type& v) const
{
	/*
		y*v.z - z * v.y
		z*v.x - x * v.z
		x*v.y - y * v.x
	*/
	return class_type(
			_mm_sub_ps(
				_mm_mul_ps(_mm_shuffle_ps(*this, *this, SHUFFLE_PARAM(1, 2, 0, 4)), _mm_shuffle_ps(v, v, SHUFFLE_PARAM(2, 0, 1, 4))),
				_mm_mul_ps(_mm_shuffle_ps(*this, *this, SHUFFLE_PARAM(2, 0, 1, 4)), _mm_shuffle_ps(v, v, SHUFFLE_PARAM(1, 2, 0, 4)))
		)
	);
}


// vector operator & : bitwise and of Vec<4, float, SIMD> and Vec4fb
/*
static FORCEINLINE Vec<4, float, SIMD> operator & (const Vec<4, float, SIMD>& a, Vec4fb const & b) {
	return _mm_and_ps(a, b);
}


static FORCEINLINE Vec<4, float, SIMD> operator & (Vec4fb const & a, const Vec<4, float, SIMD>& b) {
	return _mm_and_ps(a, b);
}


static FORCEINLINE Vec4fb operator ! (const Vec<4, float, SIMD>& a) {
	return a == Vec<4, float, SIMD>(0.0f);
}

*/

TRE_NS_END