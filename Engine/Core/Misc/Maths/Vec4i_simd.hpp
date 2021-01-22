#pragma once

#include <Engine/Core/Misc/Defines/PlatformSIMDInclude.hpp>
#include "Vec.hpp"
#include "VecN.hpp"
#include "Vec2.hpp"
#include "Vec3.hpp"

TRE_NS_START

// Select between two sources, byte by byte. Used in various functions and operators
// Corresponds to this pseudocode:
// for (int i = 0; i < 16; i++) result[i] = s[i] ? a[i] : b[i];
// Each byte in s must be either 0 (false) or 0xFF (true). No other values are allowed.
// The implementation depends on the instruction set: 
// If SSE4.1 is supported then only bit 7 in each byte of s is checked, 
// otherwise all bits in s are used.
static FORCEINLINE __m128i selectb(__m128i const & s, __m128i const & a, __m128i const & b) {
#if SIMD_SUPPORTED_LEVEL >= 5   // SSE4.1 supported
	return _mm_blendv_epi8(b, a, s);
#else
	return _mm_or_si128(
		_mm_and_si128(s, a),
		_mm_andnot_si128(s, b));
#endif
}

template<>
struct Vec<4, int32, SIMD> {
public:
	union { 
		__m128i xmm; 
		struct { int32 x, y, z, w; };  
		struct { int32 r, g, b, a; };  
		struct { int32 s, t, p, q; }; 
		int32 data[4];
		struct { Vec<2, int32, normal> xy, zw; } pos;
		struct { Vec<2, int32, normal> rg, ba; } col;
		struct { Vec<2, int32, normal> st, pq; } tex;
		Vec<3, int32, normal> xyz;
		Vec<3, int32, normal> rga;
		Vec<3, int32, normal> stp;
	}; // Float vector

	// Default constructor:
	FORCEINLINE Vec() {}

	// Constructor to broadcast the same value into all elements:
	FORCEINLINE Vec(int32 f);

	// Constructor to build from all elements:
	FORCEINLINE Vec(int32, int32, int32, int32);

	// Constructor to convert from type __m128 used in intrinsics:
	FORCEINLINE Vec(const __m128i&);

	//Destructor
	FORCEINLINE ~Vec() {};

	// Assignment operator to convert from type __m128 used in intrinsics:
	FORCEINLINE Vec& operator= (const __m128i&);

	// Type cast operator to convert to __m128 used in intrinsics
	FORCEINLINE operator __m128i() const;

	// Member function to load from array (unaligned)
	FORCEINLINE Vec& load(int32 const*);

	// Member function to load from array, aligned by 16
	// "load_a" is faster than "load" on older Intel processors (Pentium 4, Pentium M, Core 1,
	// Merom, Wolfdale) and Atom, but not on other processors from Intel, AMD or VIA.
	// You may use load_a instead of load if you are certain that p points to an address
	// divisible by 16.
	FORCEINLINE Vec& load_a(int32 const *);

	// Member function to store into array (unaligned)
	FORCEINLINE void store(int32*) const;

	// Member function to store into array, aligned by 16
	// "store_a" is faster than "store" on older Intel processors (Pentium 4, Pentium M, Core 1,
	// Merom, Wolfdale) and Atom, but not on other processors from Intel, AMD or VIA.
	// You may use store_a instead of store if you are certain that p points to an address
	// divisible by 16.
	FORCEINLINE void store_a(int32*) const;

	// Partial load. Load n elements and set the rest to 0
	/**
		@param int n : Load the lower n floating point from array p
		@param int32 const* : The array to load from
	*/
	FORCEINLINE Vec& load_partial(int, int32 const*);

	// Partial store. Store n elements
	FORCEINLINE void store_partial(int, int32*) const;

	// cut off vector to n elements. The last 4-n elements are set to zero
	FORCEINLINE Vec& cutoff(int);

	// Member function extract a single element from vector
	FORCEINLINE int32 extract(uint32) const;

	// Extract a single element. Use store function if extracting more than one element.
	// Operator [] can only read an element, not write.
	FORCEINLINE int32 operator[] (uint32) const;

	// Member function to change a single element in vector
	// Note: This function is inefficient. Use load function if changing more than one element
	FORCEINLINE const Vec& insert(uint32, int32);

	static int size() {
		return 4;
	}
};


// vector operator + : add element by element
static FORCEINLINE Vec<4, int32, SIMD> operator+(Vec<4, int32, SIMD> const & a, Vec<4, int32, SIMD> const & b);

// vector operator + : add vector and scalar

static FORCEINLINE Vec<4, int32, SIMD> operator+(Vec<4, int32, SIMD> const & a, int32 b);

static FORCEINLINE Vec<4, int32, SIMD> operator+(int32 a, Vec<4, int32, SIMD> const & b);

// vector operator += : add
static FORCEINLINE Vec<4, int32, SIMD>& operator+=(Vec<4, int32, SIMD> & a, Vec<4, int32, SIMD> const & b);

// postfix operator ++
static FORCEINLINE Vec<4, int32, SIMD> operator++(Vec<4, int32, SIMD> & a, int);

// prefix operator ++
static FORCEINLINE Vec<4, int32, SIMD>& operator++(Vec<4, int32, SIMD> & a);

// vector operator - : subtract element by element
static FORCEINLINE Vec<4, int32, SIMD> operator-(Vec<4, int32, SIMD> const & a, Vec<4, int32, SIMD> const & b);

// vector operator - : subtract vector and scalar
static FORCEINLINE Vec<4, int32, SIMD> operator-(Vec<4, int32, SIMD> const & a, int32 b);


static FORCEINLINE Vec<4, int32, SIMD> operator-(int32 a, Vec<4, int32, SIMD> const & b);

// vector operator - : unary minus
// Change sign bit, even for 0, INF and NAN
static FORCEINLINE Vec<4, int32, SIMD> operator-(Vec<4, int32, SIMD> const & a);

// vector operator -= : subtract
static FORCEINLINE Vec<4, int32, SIMD>& operator-=(Vec<4, int32, SIMD> & a, Vec<4, int32, SIMD> const & b);

// postfix operator --
static FORCEINLINE Vec<4, int32, SIMD> operator--(Vec<4, int32, SIMD> & a, int);

// prefix operator --
static FORCEINLINE Vec<4, int32, SIMD>& operator--(Vec<4, int32, SIMD> & a);

// vector operator * : multiply element by element
static FORCEINLINE Vec<4, int32, SIMD> operator*(Vec<4, int32, SIMD> const & a, Vec<4, int32, SIMD> const & b);

// vector operator * : multiply vector and scalar
static FORCEINLINE Vec<4, int32, SIMD> operator*(Vec<4, int32, SIMD> const & a, int32 b);


static FORCEINLINE Vec<4, int32, SIMD> operator*(int32 a, Vec<4, int32, SIMD> const & b);

// vector operator *= : multiply
static FORCEINLINE Vec<4, int32, SIMD>& operator*=(Vec<4, int32, SIMD> & a, Vec<4, int32, SIMD> const & b);

// vector operator | : bitwise or
static FORCEINLINE Vec<4, int32, SIMD> operator|(Vec<4, int32, SIMD> const & a, Vec<4, int32, SIMD> const & b);

// vector operator |= : bitwise or
static FORCEINLINE Vec<4, int32, SIMD>& operator|=(Vec<4, int32, SIMD> & a, Vec<4, int32, SIMD> const & b);

// vector operator ^ : bitwise xor
static FORCEINLINE Vec<4, int32, SIMD> operator^(Vec<4, int32, SIMD> const & a, Vec<4, int32, SIMD> const & b);

// vector operator ^= : bitwise xor
static FORCEINLINE Vec<4, int32, SIMD>& operator^=(Vec<4, int32, SIMD> & a, Vec<4, int32, SIMD> const & b);

// vector operator == : returns true for elements for which a == b
/*
static FORCEINLINE Vec4fb operator == (Vec<4, int32, SIMD> const & a, Vec<4, int32, SIMD> const & b);

// vector operator != : returns true for elements for which a != b
static FORCEINLINE Vec4fb operator != (Vec<4, int32, SIMD> const & a, Vec<4, int32, SIMD> const & b);

// vector operator < : returns true for elements for which a < b
static FORCEINLINE Vec4fb operator < (Vec<4, int32, SIMD> const & a, Vec<4, int32, SIMD> const & b);

// vector operator <= : returns true for elements for which a <= b
static FORCEINLINE Vec4fb operator <= (Vec<4, int32, SIMD> const & a, Vec<4, int32, SIMD> const & b);

// vector operator > : returns true for elements for which a > b
static FORCEINLINE Vec4fb operator > (Vec<4, int32, SIMD> const & a, Vec<4, int32, SIMD> const & b);

// vector operator >= : returns true for elements for which a >= b
static FORCEINLINE Vec4fb operator >= (Vec<4, int32, SIMD> const & a, Vec<4, int32, SIMD> const & b);

// Bitwise logical operators

// vector operator & : bitwise and

static FORCEINLINE Vec<4, int32, SIMD> operator & (Vec<4, int32, SIMD> const & a, Vec<4, int32, SIMD> const & b);

// vector operator &= : bitwise and

static FORCEINLINE Vec<4, int32, SIMD> & operator &= (Vec<4, int32, SIMD> & a, Vec<4, int32, SIMD> const & b);


// vector operator & : bitwise and of Vec<4, int32, SIMD> and Vec4fb
static FORCEINLINE Vec<4, int32, SIMD> operator & (Vec<4, int32, SIMD> const & a, Vec4fb const & b);


static FORCEINLINE Vec<4, int32, SIMD> operator & (Vec4fb const & a, Vec<4, int32, SIMD> const & b);
*/

/*// vector operator ! : logical not. Returns Boolean vector

static FORCEINLINE Vec4fb operator ! (Vec<4, int32, SIMD> const & a) {
	return a == Vec<4, int32, SIMD>(0.0f);
}*/

TRE_NS_END

#include "Vec4i_simd.inl"