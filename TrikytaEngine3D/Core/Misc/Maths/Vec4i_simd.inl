#include "Vec4i_simd.hpp"

TRE_NS_START

Vec<4, int32, SIMD>::Vec(int32 i)
{
	xmm = _mm_set1_epi32(i);
}

Vec<4, int32, SIMD>::Vec(int32 x, int32 y, int32 z, int32 w)
{
	xmm = _mm_setr_epi32(x, y, z, w);
}

Vec<4, int32, SIMD>::Vec(const __m128i&  x)
{
	xmm = x;
}

FORCEINLINE Vec<4, int32, SIMD>&  Vec<4, int32, SIMD>::operator= (const __m128i& x)
{
	xmm = x;
	return *this;
}

FORCEINLINE Vec<4, int32, SIMD>::operator __m128i() const
{
	return xmm;
}

FORCEINLINE Vec<4, int32, SIMD>&  Vec<4, int32, SIMD>::load(int32 const* p)
{
	xmm = _mm_loadu_si128((__m128i const*)p);
	return *this;
}

FORCEINLINE  Vec<4, int32, SIMD>&  Vec<4, int32, SIMD>::load_a(int32 const * p)
{
	xmm = _mm_load_si128((__m128i const*)p);
	return *this;
}

FORCEINLINE void  Vec<4, int32, SIMD>::store(int32* p) const
{
	_mm_storeu_si128((__m128i*)p, xmm);
}

FORCEINLINE void  Vec<4, int32, SIMD>::store_a(int32* p) const
{
	_mm_store_si128((__m128i*)p, xmm);
}

FORCEINLINE  Vec<4, int32, SIMD>&  Vec<4, int32, SIMD>::load_partial(int n, int32 const* p)
{
	switch (n) {
	case 0:
		*this = 0;  break;
	case 1:
		xmm = _mm_cvtsi32_si128(*(int32_t const*)p);  break;
	case 2:
		// intrinsic for movq is missing!
		xmm = _mm_setr_epi32(((int32_t const*)p)[0], ((int32_t const*)p)[1], 0, 0);  break;
	case 3:
		xmm = _mm_setr_epi32(((int32_t const*)p)[0], ((int32_t const*)p)[1], ((int32_t const*)p)[2], 0);  break;
	case 4:
		load(p);  break;
	default:
		break;
	}
	return *this;
}

FORCEINLINE void  Vec<4, int32, SIMD>::store_partial(int n, int32 * p) const 
{
	union {
		int32_t i[4];
		int64_t q[2];
	} u;
	switch (n) {
	case 1:
		*(int32_t*)p = _mm_cvtsi128_si32(xmm);  break;
	case 2:
		// intrinsic for movq is missing!
		store(u.i);
		*(int64_t*)p = u.q[0];  break;
	case 3:
		store(u.i);
		*(int64_t*)p = u.q[0];
		((int32_t*)p)[2] = u.i[2];  break;
	case 4:
		store(p);  break;
	default:
		break;
	}
}

FORCEINLINE  Vec<4, int32, SIMD>&  Vec<4, int32, SIMD>::cutoff(int n)
{
	return *this;
}


FORCEINLINE const  Vec<4, int32, SIMD>&  Vec<4, int32, SIMD>::insert(uint32 index, int32 value)
{
	static const int32 maskl[8] = { 0,0,0,0,-1,0,0,0 };
	__m128i broad = _mm_set1_epi32(value);  // broadcast value into all elements
	__m128i mask = _mm_loadu_si128((__m128i const*)(maskl + 4 - (index & 3))); // mask with FFFFFFFF at index position
	xmm = selectb(mask, broad, xmm);
	return *this;
};

FORCEINLINE int32  Vec<4, int32, SIMD>::extract(uint32 index) const
{
	int32 x[4];
	store(x);
	return x[index & 3];
}

FORCEINLINE int32  Vec<4, int32, SIMD>::operator[](uint32 index) const
{
	return extract(index);
}

static FORCEINLINE  Vec<4, int32, SIMD> operator+(const  Vec<4, int32, SIMD>& a, const  Vec<4, int32, SIMD>& b) {
	return _mm_add_epi32(a, b);
}

static FORCEINLINE  Vec<4, int32, SIMD> operator+(const  Vec<4, int32, SIMD>& a, int32 b) {
	return a +  Vec<4, int32, SIMD>(b);
}

static FORCEINLINE  Vec<4, int32, SIMD> operator+(int32 a, const  Vec<4, int32, SIMD>& b) {
	return  Vec<4, int32, SIMD>(a) + b;
}

static FORCEINLINE  Vec<4, int32, SIMD>& operator+=( Vec<4, int32, SIMD>& a, const  Vec<4, int32, SIMD>& b) {
	a = a + b;
	return a;
}

static FORCEINLINE  Vec<4, int32, SIMD> operator++( Vec<4, int32, SIMD>& a, int) {
	 Vec<4, int32, SIMD> a0 = a;
	a = a + 1;
	return a0;
}

static FORCEINLINE  Vec<4, int32, SIMD>& operator++( Vec<4, int32, SIMD>& a) {
	a = a + 1;
	return a;
}

static FORCEINLINE  Vec<4, int32, SIMD> operator-(const  Vec<4, int32, SIMD>& a, const  Vec<4, int32, SIMD>& b) {
	return _mm_sub_epi32(a, b);
}

static FORCEINLINE  Vec<4, int32, SIMD> operator-(const  Vec<4, int32, SIMD>& a, int32 b) {
	return a -  Vec<4, int32, SIMD>(b);
}

static FORCEINLINE  Vec<4, int32, SIMD> operator-(int32 a, const  Vec<4, int32, SIMD>& b) {
	return  Vec<4, int32, SIMD>(a) - b;
}

static FORCEINLINE  Vec<4, int32, SIMD> operator-(const  Vec<4, int32, SIMD>& a) {
	return _mm_sub_epi32(_mm_setzero_si128(), a);
}

static FORCEINLINE  Vec<4, int32, SIMD>& operator-=( Vec<4, int32, SIMD>& a, const  Vec<4, int32, SIMD>& b) {
	a = a - b;
	return a;
}

static FORCEINLINE  Vec<4, int32, SIMD> operator--( Vec<4, int32, SIMD>& a, int) {
	 Vec<4, int32, SIMD> a0 = a;
	a = a - 1;
	return a0;
}

static FORCEINLINE  Vec<4, int32, SIMD>& operator--( Vec<4, int32, SIMD>& a) {
	a = a - 1;
	return a;
}

static FORCEINLINE  Vec<4, int32, SIMD> operator*(const  Vec<4, int32, SIMD>& a, const  Vec<4, int32, SIMD>& b) {
#if SIMD_SUPPORTED_LEVEL >= 5  // SSE4.1 instruction set
	return _mm_mullo_epi32(a, b);
#else
	__m128i a13 = _mm_shuffle_epi32(a, 0xF5);			  // (-,a3,-,a1)
	__m128i b13 = _mm_shuffle_epi32(b, 0xF5);			  // (-,b3,-,b1)
	__m128i prod02 = _mm_mul_epu32(a, b);                 // (-,a2*b2,-,a0*b0)
	__m128i prod13 = _mm_mul_epu32(a13, b13);             // (-,a3*b3,-,a1*b1)
	__m128i prod01 = _mm_unpacklo_epi32(prod02, prod13);  // (-,-,a1*b1,a0*b0) 
	__m128i prod23 = _mm_unpackhi_epi32(prod02, prod13);  // (-,-,a3*b3,a2*b2) 
	return           _mm_unpacklo_epi64(prod01, prod23);  // (ab3,ab2,ab1,ab0)
#endif
}

static FORCEINLINE  Vec<4, int32, SIMD> operator*(const  Vec<4, int32, SIMD>& a, int32 b) {
	return a *  Vec<4, int32, SIMD>(b);
}

static FORCEINLINE  Vec<4, int32, SIMD> operator*(int32 a, const  Vec<4, int32, SIMD>& b) {
	return  Vec<4, int32, SIMD>(a) * b;
}

static FORCEINLINE  Vec<4, int32, SIMD>& operator*=( Vec<4, int32, SIMD>& a, const  Vec<4, int32, SIMD>& b) {
	a = a * b;
	return a;
}

static FORCEINLINE  Vec<4, int32, SIMD> operator&(const  Vec<4, int32, SIMD>& a, const  Vec<4, int32, SIMD>& b) {
	return Vec<4, int32, SIMD>(_mm_and_si128(a, b));
}

static FORCEINLINE  Vec<4, int32, SIMD>& operator&=( Vec<4, int32, SIMD>& a, const  Vec<4, int32, SIMD>& b) {
	a = a & b;
	return a;
}


static FORCEINLINE  Vec<4, int32, SIMD> operator|(const  Vec<4, int32, SIMD>& a, const  Vec<4, int32, SIMD>& b) {
	return Vec<4, int32, SIMD>(_mm_or_si128(a, b));
}


static FORCEINLINE  Vec<4, int32, SIMD>& operator|=( Vec<4, int32, SIMD>& a, const  Vec<4, int32, SIMD>& b) {
	a = a | b;
	return a;
}


static FORCEINLINE  Vec<4, int32, SIMD> operator^(const  Vec<4, int32, SIMD>& a, const  Vec<4, int32, SIMD>& b) {
	return  Vec<4, int32, SIMD>(_mm_xor_si128(a, b));
}

static FORCEINLINE  Vec<4, int32, SIMD>& operator^=( Vec<4, int32, SIMD>& a, const  Vec<4, int32, SIMD>& b) {
	a = a ^ b;
	return a;
}

static FORCEINLINE bool operator== (const  Vec<4, int32, SIMD>& a, const  Vec<4, int32, SIMD>& b) {
	_m128i bres = _mm_cmpeq_epi32(a, b);
	return unsigned(bres.m128i_i32[0]) == 0xffffffff && unsigned(bres.m128i_i32[1]) == 0xffffffff && unsigned(bres.m128i_i32[2]) == 0xffffffff && unsigned(bres.m128i_i32[3]) == 0xffffffff;
}


static FORCEINLINE bool operator!=(const  Vec<4, int32, SIMD>& a, const  Vec<4, int32, SIMD>& b) {
	return !(a == b);
}

static FORCEINLINE bool operator>(const  Vec<4, int32, SIMD>& a, const  Vec<4, int32, SIMD>& b) {
	_m128i bres = _mm_cmpgt_epi32(a, b);
	return unsigned(bres.m128i_i32[0]) == 0xffffffff && unsigned(bres.m128i_i32[1]) == 0xffffffff && unsigned(bres.m128i_i32[2]) == 0xffffffff && unsigned(bres.m128i_i32[3]) == 0xffffffff;
}

static FORCEINLINE bool operator<(const  Vec<4, int32, SIMD>& a, const  Vec<4, int32, SIMD>& b) {
	return b > a;
}

static FORCEINLINE bool operator<=(const  Vec<4, int32, SIMD>& a, const  Vec<4, int32, SIMD>& b) {
	return ( a < b) || (a == b);
}

static FORCEINLINE bool operator>=(const  Vec<4, int32, SIMD>& a, const  Vec<4, int32, SIMD>& b) {
	return (b <= a);
}

// vector operator << : shift left
static FORCEINLINE Vec<4, int32, SIMD> operator<<(Vec<4, int32, SIMD> const & a, int32_t b) {
	return _mm_sll_epi32(a, _mm_cvtsi32_si128(b));
}

// vector operator <<= : shift left
static FORCEINLINE Vec<4, int32, SIMD>& operator<<= (Vec<4, int32, SIMD>& a, int32_t b) {
	a = a << b;
	return a;
}

// vector operator >> : shift right arithmetic
static FORCEINLINE Vec<4, int32, SIMD> operator>> (const Vec<4, int32, SIMD>& a, int32_t b) {
	return _mm_sra_epi32(a, _mm_cvtsi32_si128(b));
}

// vector operator >>= : shift right arithmetic
static FORCEINLINE Vec<4, int32, SIMD>& operator >>= (Vec<4, int32, SIMD>& a, int32_t b) {
	a = a >> b;
	return a;
}

// vector operator ~ : bitwise not
static FORCEINLINE Vec<4, int32, SIMD>operator~ (const Vec<4, int32, SIMD>& a) {
	return Vec<4, int32, SIMD>(_mm_xor_si128(a, _mm_set1_epi32(-1)));
}

TRE_NS_END