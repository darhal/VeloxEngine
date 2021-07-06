//#pragma once
//
//#include "Vec.hpp"
//#include "VecN.hpp"
//#include "Vec2.hpp"
//#include "Vec3.hpp"
//#include <Legacy/Misc/Defines/PlatformSIMDInclude.hpp>
//
//TRE_NS_START
//
//typedef Vec<4, float, SIMD> Vec4f;
//
//template<>
//struct Vec<4, float, SIMD>{
//public:
//	typedef Vec<4, float, SIMD> class_type;
//
//	union { 
//		__m128 xmm; 
//		struct { float x, y, z, w; };  
//		struct { float r, g, b, a; }; 
//		struct { float s, t, p, q; }; 
//		float data[4];
//		struct { Vec<2, float, normal> xy, zw; } pos;
//		struct { Vec<2, float, normal> rg, ba; } col;
//		struct { Vec<2, float, normal> st, pq; } tex;
//		Vec<3, float, normal> xyz;
//		Vec<3, float, normal> rga;
//		Vec<3, float, normal> stp;
//	}; // Float vector
//
//	// Default constructor:
//	FORCEINLINE Vec() : xmm() {};
//
//	// Constructor to broadcast the same value into all elements:
//	FORCEINLINE  Vec(float f);
//
//	// Constructor to build from all elements:
//	FORCEINLINE  Vec(float, float, float, float);
//
//	// Constructor to convert from type __m128 used in intrinsics:
//	FORCEINLINE  Vec(const __m128&);
//
//	//Destructor
//	~Vec() {};
//
//	// Assignment operator to convert from type __m128 used in intrinsics:
//	FORCEINLINE Vec& operator=(const __m128&);
//
//	// Type cast operator to convert to __m128 used in intrinsics
//	FORCEINLINE operator __m128() const;
//
//	// Member function to load from array (unaligned)
//	FORCEINLINE Vec& load(float const*);
//
//	// Member function to load from array, aligned by 16
//	// "load_a" is faster than "load" on older Intel processors (Pentium 4, Pentium M, Core 1,
//	// Merom, Wolfdale) and Atom, but not on other processors from Intel, AMD or VIA.
//	// You may use load_a instead of load if you are certain that p points to an address
//	// divisible by 16.
//	FORCEINLINE Vec& load_a(float const *);
//
//	// Member function to store into array (unaligned)
//	FORCEINLINE void store(float*) const;
//
//	// Member function to store into array, aligned by 16
//	// "store_a" is faster than "store" on older Intel processors (Pentium 4, Pentium M, Core 1,
//	// Merom, Wolfdale) and Atom, but not on other processors from Intel, AMD or VIA.
//	// You may use store_a instead of store if you are certain that p points to an address
//	// divisible by 16.
//	FORCEINLINE void store_a(float*) const;
//
//	// Partial load. Load n elements and set the rest to 0
//	/**
//		@param int n : Load the lower n floating point from array p
//		@param float const* : The array to load from
//	*/
//	FORCEINLINE Vec& load_partial(int, float const*);
//
//	// Partial store. Store n elements
//	FORCEINLINE void store_partial(int, float*) const;
//
//	// cut off vector to n elements. The last 4-n elements are set to zero
//	FORCEINLINE Vec& cutoff(int);
//
//	// Member function extract a single element from vector
//	FORCEINLINE float extract(uint32) const;
//
//	// Extract a single element. Use store function if extracting more than one element.
//	// Operator [] can only read an element, not write.
//	FORCEINLINE float operator[] (uint32) const;
//
//	// Member function to change a single element in vector
//	// Note: This function is inefficient. Use load function if changing more than one element
//	FORCEINLINE const Vec& insert(uint32, float);
//
//
//	/*OTHER OPS*/
//	FORCEINLINE const Vec<4, float, SIMD> projection(const class_type&) const;
//	FORCEINLINE const float scalar_projection(const class_type&) const;
//
//	FORCEINLINE float dot_product(const class_type&) const;
//	FORCEINLINE float angle(const class_type&) const;
//
//	FORCEINLINE float lengthSqr() const;
//	FORCEINLINE float length() const;
//	FORCEINLINE float distance(const class_type&) const;
//	FORCEINLINE const Vec<4, float, SIMD> unit() const;
//	FORCEINLINE const Vec<4, float, SIMD> cross_product(const class_type&) const;
//
//	static int size() {
//		return 4;
//	}
//};
//
//
//// vector operator + : add element by element
//static FORCEINLINE Vec<4, float, SIMD> operator+(Vec<4, float, SIMD> const & a, Vec<4, float, SIMD> const & b);
//
//// vector operator + : add vector and scalar
//
//static FORCEINLINE Vec<4, float, SIMD> operator+(Vec<4, float, SIMD> const & a, float b);
//
//static FORCEINLINE Vec<4, float, SIMD> operator+(float a, Vec<4, float, SIMD> const & b);
//
//// vector operator += : add
//static FORCEINLINE Vec<4, float, SIMD> & operator+=(Vec<4, float, SIMD> & a, Vec<4, float, SIMD> const & b);
//
//// postfix operator ++
//static FORCEINLINE Vec<4, float, SIMD> operator++(Vec<4, float, SIMD> & a, int);
//
//// prefix operator ++
//static FORCEINLINE Vec<4, float, SIMD> & operator++(Vec<4, float, SIMD> & a);
//
//// vector operator - : subtract element by element
//static FORCEINLINE Vec<4, float, SIMD> operator-(Vec<4, float, SIMD> const & a, Vec<4, float, SIMD> const & b);
//
//// vector operator - : subtract vector and scalar
//static FORCEINLINE Vec<4, float, SIMD> operator-(Vec<4, float, SIMD> const & a, float b);
//
//
//static FORCEINLINE Vec<4, float, SIMD> operator-(float a, Vec<4, float, SIMD> const & b);
//
//// vector operator - : unary minus
//// Change sign bit, even for 0, INF and NAN
//static FORCEINLINE Vec<4, float, SIMD> operator-(Vec<4, float, SIMD> const & a);
//
//// vector operator -= : subtract
//static FORCEINLINE Vec<4, float, SIMD> & operator-=(Vec<4, float, SIMD> & a, Vec<4, float, SIMD> const & b);
//
//// postfix operator --
//static FORCEINLINE Vec<4, float, SIMD> operator--(Vec<4, float, SIMD> & a, int);
//
//// prefix operator --
//static FORCEINLINE Vec<4, float, SIMD> & operator--(Vec<4, float, SIMD> & a);
//
//// vector operator * : multiply element by element
//static FORCEINLINE Vec<4, float, SIMD> operator*(Vec<4, float, SIMD> const & a, Vec<4, float, SIMD> const & b);
//
//// vector operator * : multiply vector and scalar
//static FORCEINLINE Vec<4, float, SIMD> operator*(Vec<4, float, SIMD> const & a, float b);
//
//
//static FORCEINLINE Vec<4, float, SIMD> operator*(float a, Vec<4, float, SIMD> const & b);
//
//// vector operator *= : multiply
//static FORCEINLINE Vec<4, float, SIMD> & operator*=(Vec<4, float, SIMD> & a, Vec<4, float, SIMD> const & b);
//
//// vector operator / : divide all elements by same integer
//static FORCEINLINE Vec<4, float, SIMD> operator/(Vec<4, float, SIMD> const & a, Vec<4, float, SIMD> const & b);
//
//// vector operator / : divide vector and scalar
//static FORCEINLINE Vec<4, float, SIMD> operator/(Vec<4, float, SIMD> const & a, float b);
//
//
//static FORCEINLINE Vec<4, float, SIMD> operator/(float a, Vec<4, float, SIMD> const & b);
//
//// vector operator /= : divide
//static FORCEINLINE Vec<4, float, SIMD> & operator/=(Vec<4, float, SIMD> & a, Vec<4, float, SIMD> const & b);
//
//// vector operator | : bitwise or
//static FORCEINLINE Vec<4, float, SIMD> operator|(Vec<4, float, SIMD> const & a, Vec<4, float, SIMD> const & b);
//
//// vector operator |= : bitwise or
//static FORCEINLINE Vec<4, float, SIMD> & operator|=(Vec<4, float, SIMD> & a, Vec<4, float, SIMD> const & b);
//
//// vector operator ^ : bitwise xor
//static FORCEINLINE Vec<4, float, SIMD> operator^(Vec<4, float, SIMD> const & a, Vec<4, float, SIMD> const & b);
//
//// vector operator ^= : bitwise xor
//static FORCEINLINE Vec<4, float, SIMD> & operator^=(Vec<4, float, SIMD> & a, Vec<4, float, SIMD> const & b);
//
//
//// vector operator == : returns true for elements for which a == b
///*
//static FORCEINLINE Vec4fb operator == (Vec<4, float, SIMD> const & a, Vec<4, float, SIMD> const & b);
//
//// vector operator != : returns true for elements for which a != b
//static FORCEINLINE Vec4fb operator != (Vec<4, float, SIMD> const & a, Vec<4, float, SIMD> const & b);
//
//// vector operator < : returns true for elements for which a < b
//static FORCEINLINE Vec4fb operator < (Vec<4, float, SIMD> const & a, Vec<4, float, SIMD> const & b);
//
//// vector operator <= : returns true for elements for which a <= b
//static FORCEINLINE Vec4fb operator <= (Vec<4, float, SIMD> const & a, Vec<4, float, SIMD> const & b);
//
//// vector operator > : returns true for elements for which a > b
//static FORCEINLINE Vec4fb operator > (Vec<4, float, SIMD> const & a, Vec<4, float, SIMD> const & b);
//
//// vector operator >= : returns true for elements for which a >= b
//static FORCEINLINE Vec4fb operator >= (Vec<4, float, SIMD> const & a, Vec<4, float, SIMD> const & b);
//
//// Bitwise logical operators
//
//// vector operator & : bitwise and
//
//static FORCEINLINE Vec<4, float, SIMD> operator & (Vec<4, float, SIMD> const & a, Vec<4, float, SIMD> const & b);
//
//// vector operator &= : bitwise and
//
//static FORCEINLINE Vec<4, float, SIMD> & operator &= (Vec<4, float, SIMD> & a, Vec<4, float, SIMD> const & b);
//
//
//// vector operator & : bitwise and of Vec<4, float, SIMD> and Vec4fb
//static FORCEINLINE Vec<4, float, SIMD> operator & (Vec<4, float, SIMD> const & a, Vec4fb const & b);
//
//
//static FORCEINLINE Vec<4, float, SIMD> operator & (Vec4fb const & a, Vec<4, float, SIMD> const & b);
//*/
//
///*// vector operator ! : logical not. Returns Boolean vector
//
//static FORCEINLINE Vec4fb operator ! (Vec<4, float, SIMD> const & a) {
//	return a == Vec<4, float, SIMD>(0.0f);
//}*/
//
//TRE_NS_END
//
//#include "Vec4f_simd.inl"