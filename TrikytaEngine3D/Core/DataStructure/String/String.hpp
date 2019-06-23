#pragma once

/************************************************************************************************************
										* Trikyta Engine 3D *

BasicString class is written using the technique that uses small string optimisation :
	Resources : https://youtu.be/kPR8h4-qZdk

Small String : Static buffer that is already on the stack, Its size is either
24 bytes or 12 bytes (depends on the architecture). When we are in the small string
case we can already anticpate the capacity because we know its either 24 or 12
so no need to store the capcity in this casee (in case of SSO). length needs to be calculated
and its the capacity - the remainder (which is stored in the last index of the static buffer)
so when its 0 it acts like null terminator. in this remainder we store also a bit
to indicate wether our string is small or not.
imagine the remainder is like this : 00001010
							^-> this is actually what indicates if our string is small or not
so we can check later if its small or not by using logical and with 00000000
if the result is 0 its small otherwise its normal.

Normal String: In the other hand normal string is just like any casual strings
the pointer point to heap buffer that have the data with its null terminator the capacity is stores
as full integer, the length however is the length of the string + a bit to indicate
its not small smthg like this (remmber its 4 bytes (or 8))
Lets imagine its 4 here
				   |-> this bit is one (it means its normal string)
0.............0 (0 0 0 0 1 0 1 1)  : m_Length bit representation in memory
^ many zeros  ^  ^  last byte  ^

************************************************************************************************************/

#include <Core/Misc/Defines/Common.hpp>
#include <Core/Misc/Defines/Debug.hpp>
#include <Core/Misc/Maths/Utils.hpp>
#include <limits>
#include <type_traits>
#include <ostream>

#if defined(COMPILER_MSVC)
	#pragma warning(disable : 4201) // Remove namesless struct/union warnning
#elif defined(COMPILER_GCC)
	#pragma GCC diagnostic ignored "-Wpedantic"
#elif defined(COMPILER_CLANG)
	#pragma clang diagnostic ignored "-Wgnu-anonymous-struct"
	#pragma clang diagnostic ignored "-Wnested-anon-types"
#endif

TRE_NS_START

template<typename T>
class BasicString
{
private:
	typedef BasicString<T> CLASS_TYPE;
	typedef typename std::make_unsigned<T>::type UT;
public:
	typedef T* Iterator;
	typedef T& RefIterator;
	typedef const T* CIterator;

	BasicString();
	BasicString(usize capacity);
	template<usize S>
	BasicString(const T(&str)[S]);
	template<usize S>
	BasicString(const T(&str)[S], usize capacity);

	BasicString(const char* str, usize S);

	~BasicString();

	// Copy ctor copy assignement
	BasicString(const BasicString<T>& other);
	BasicString<T>& operator=(const BasicString<T>& other);

	// Move ctor move assignement
	BasicString(BasicString<T>&& other);
	BasicString<T>& operator=(BasicString<T>&& other);

	FORCEINLINE void Reserve(usize s);
	FORCEINLINE void Clear();
	void Resize(usize s);
	
	void Append(const BasicString<T>& str);
	void Append(const char* str);
	void PushBack(T c);
	void PopBack();
	void Erase(usize pos, usize offset);
	void Insert(usize pos, const BasicString<T>& str);
	void Copy(const BasicString<T>& str, usize pos, usize offset);

	FORCEINLINE const T*	   Buffer()							    const;
	FORCEINLINE usize		   Length()							    const;
	FORCEINLINE usize		   Size()								const;
	FORCEINLINE usize	       Capacity()							const;
	FORCEINLINE T			   At(usize i)							const;
	FORCEINLINE T			   Back()								const;
	FORCEINLINE T			   Front()								const;
	FORCEINLINE ssize		   Find(const BasicString<T>& pattren)  const;
	FORCEINLINE BasicString<T> SubString(usize pos, usize off)	    const;
	FORCEINLINE bool		   IsEmpty()							const;


	FORCEINLINE Iterator begin() noexcept;
	FORCEINLINE Iterator end() noexcept;

	template<typename NUMERIC_TYPE>
	T& operator[](const NUMERIC_TYPE i);
	template<typename NUMERIC_TYPE>
	const T& operator[](const NUMERIC_TYPE i) const;

	FORCEINLINE BasicString<T>& operator+=(const BasicString<T>& other);
public:
	CONSTEXPR static usize SPARE_RATE = 2;
//private:
	CONSTEXPR static usize SSO_SIZE	  = (sizeof(T*) + 2 * sizeof(usize)) / sizeof(T); 
	CONSTEXPR static usize SSO_MI	  = SSO_SIZE - 1;  // MI stands for Max Index.
	
#if ENDIANNESS == LITTLE_ENDIAN
	CONSTEXPR static usize hight_bit_mask_normal_str = usize(1) << (sizeof(usize)*BITS_PER_BYTE - 1);
#else
#endif

	union {
		struct {
			T*		m_Buffer;
			usize	m_Capacity;
			usize	m_Length;
		};
		struct {
			T m_Data[SSO_SIZE]; //SSO optimization
		};
	};

	FORCEINLINE bool IsSmall() const;
	FORCEINLINE void SetLength(usize nlen);
	FORCEINLINE void SetSmallLength(usize nlen);
	FORCEINLINE void SetNormalLength(usize nlen);
	FORCEINLINE void SetCapacity(usize ncap);
	FORCEINLINE T*   EditableBuffer();

	template<usize S, bool IsSmall>
	struct InitString {};

	template<usize S>
	struct InitString<S, true>
	{
		void operator()(BasicString<T>* obj, const T(&str)[S])
		{
			for (usize i = 0; i < S; i++) {
				obj->m_Data[i] = str[i];
			}
			obj->SetSmallLength(S);
		}
	};

	template<usize S>
	struct InitString<S, false>
	{
		void operator()(BasicString<T>* obj, const T(&str)[S])
		{
			usize real_cap = S * SPARE_RATE;
			obj->m_Buffer = (T*) operator new (sizeof(T)* real_cap); // allocate empty storage
			for (usize i = 0; i < S; i++) {
				obj->m_Buffer[i] = str[i];
			}
			obj->m_Capacity = real_cap;
			obj->SetNormalLength(S); // bit to indicate that its not small string 
								// (the bit is located always at the end of the bit stream, 
								// works both on little and big endian architectures)
		}
	};
};

template<typename T>
template<usize S>
BasicString<T>::BasicString(const T(&str)[S])
{
	BasicString<T>::InitString<S, S <= SSO_SIZE>()(this, str);
	/*if (S <= SSO_SIZE) { // here its <= because we will count the trailing null
		for (usize i = 0; i < S; i++) {
			m_Data[i] = str[i];
		}
		SetSmallLength(S);
	}else{
		usize real_cap = S * SPARE_RATE;
		m_Buffer = (T*) operator new (sizeof(T)* real_cap); // allocate empty storage
		for (usize i = 0; i < S; i++) {
			m_Buffer[i] = str[i];
		}
		m_Capacity = real_cap;
		SetNormalLength(S); // bit to indicate that its not small string 
							// (the bit is located always at the end of the bit stream, 
							// works both on little and big endian architectures)
	}*/
}

template<typename T>
template<usize S>
BasicString<T>::BasicString(const T(&str)[S], usize capacity)
{
	if (capacity < S) capacity = S;
	if (capacity <= SSO_SIZE) { // here its <= because we will count the trailing null
		for (usize i = 0; i < S; i++) {
			m_Data[i] = str[i];
		}
		SetSmallLength(S);
	}else{
		usize real_cap = capacity * SPARE_RATE;
		m_Buffer = (T*) operator new (sizeof(T)* real_cap); // allocate empty storage
		for (usize i = 0; i < S; i++) {
			m_Buffer[i] = str[i];
		}
		m_Capacity = real_cap;
		SetNormalLength(S); // bit to indicate that its not small string 
							// (the bit is located always at the end of the bit stream, 
							// works both on little and big endian architectures)
	}
}

template<typename T>
BasicString<T>::BasicString(const char* str, usize S)
{
	usize len = strlen(str);
	if (S < len) S = len;
	if (S <= SSO_SIZE) { // here its <= because we will count the trailing null
		for (usize i = 0; i < len; i++) {
			m_Data[i] = str[i];
		}
		SetSmallLength(len);
	}else{
		usize real_cap = S * SPARE_RATE;
		m_Buffer = (T*) operator new (sizeof(T)* real_cap); // allocate empty storage
		for (usize i = 0; i < len; i++) {
			m_Buffer[i] = str[i];
		}
		m_Capacity = real_cap;
		SetNormalLength(len); // bit to indicate that its not small string 
							// (the bit is located always at the end of the bit stream, 
							// works both on little and big endian architectures)
	}
}

template<typename T>
template<typename NUMERIC_TYPE>
T& BasicString<T>::operator[] (const NUMERIC_TYPE i)
{
	ASSERTF(!(Absolute(i) > (ssize)m_Length), "Bad usage of [] with BasicString class, given index out of bounds.");
	return this->EditableBuffer[i];
}

template<typename T>
template<typename NUMERIC_TYPE>
const T& BasicString<T>::operator[] (const NUMERIC_TYPE i) const
{
	ASSERTF(!(Absolute(i) > (ssize)m_Length), "Bad usage of [] with BasicString class, given index out of bounds.");
	return this->Buffer[i];
}

// The preprocessing function for Boyer Moore's  
// bad character heuristic  
template<typename T, usize NB_CHAR>
void BadCharHeuristic(const BasicString<T>& str, ssize size, int32(&badchar)[NB_CHAR])
{
	int32 i;
	// Initialize all occurrences as -1  
	for (i = 0; i < NB_CHAR; i++)
		badchar[i] = -1;

	// Fill the actual value of last occurrence  
	// of a character  
	const T* str_buffer = str.Buffer();
	for (i = 0; i < size; i++)
		badchar[(int)str_buffer[i]] = i;
}

/* A pattern searching function that uses Bad
Character Heuristic of Boyer Moore Algorithm */
template<typename T>
ssize SearchBoyerMoore(const BasicString<T>& txt, const BasicString<T>& pat)
{
	ssize m = (ssize)pat.Length();
	ssize n = (ssize)txt.Length();
	const T* txt_buffer = txt.Buffer();
	const T* pat_buffer = pat.Buffer();

	const usize NB_OF_CHARS = std::numeric_limits<T>::max() + ABS(std::numeric_limits<T>::min()) + 1;
	int32 badchar[NB_OF_CHARS];

	/* Fill the bad character array by calling
	the preprocessing function BadCharHeuristic()
	for given pattern */
	BadCharHeuristic(pat, m, badchar);

	ssize s = 0; // s is shift of the pattern with  
				// respect to text  
	while (s <= (n - m))
	{
		ssize j = m - 1;

		/* Keep reducing index j of pattern while
		characters of pattern and text are
		matching at this shift s */
		while (j >= 0 && pat_buffer[j] == txt_buffer[s + j])
			j--;

		/* If the pattern is present at current
		shift, then index j will become -1 after
		the above loop */
		if (j < 0)
		{
			/* Shift the pattern so that the next
			character in text aligns with the last
			occurrence of it in pattern.
			The condition s+m < n is necessary for
			the case when pattern occurs at the end
			of text */
			return s; // cout << "pattern occurs at shift = " <<  s << endl;  
			// s += (s + m < n) ? m - badchar[txt_buffer[s + m]] : 1;

		} else {
			/* Shift the pattern so that the bad character
			in text aligns with the last occurrence of
			it in pattern. The max function is used to
			make sure that we get a positive shift.
			We may get a negative shift if the last
			occurrence of bad character in pattern
			is on the right side of the current
			character. */
			s += MAX(1, j - badchar[txt_buffer[s + j]]);
		}
	}
	return -1;
}

/********************NON CLASS OPS********************/
template<typename T>
static bool operator==(BasicString<T>&& a, BasicString<T>&& b)
{
	usize alen = a.Length();
	const T* a_buffer = a.Buffer();
	const T* b_buffer = b.Buffer();
	if (alen != b.Length()) return false;
	for (usize i = 0; i < alen - 1; i++) {
		if (a_buffer[i] != b_buffer[i]) {
			return false;
		}
	}
	return true;
}

template<typename T>
static bool operator!=(BasicString<T>&& a, BasicString<T>&& b)
{
	return !(a == b);
}

template<typename T>
static bool operator==(const BasicString<T>& a, const BasicString<T>& b)
{
	usize alen = a.Length();
	const T* a_buffer = a.Buffer();
	const T* b_buffer = b.Buffer();
	if (alen != b.Length()) return false;
	for (usize i = 0; i < alen - 1; i++) {
		if (a_buffer[i] != b_buffer[i]) {
			return false;
		}
	}
	return true;
}

template<typename T>
static bool operator!=(const BasicString<T>& a, const BasicString<T>& b)
{
	return !(a == b);
}

template<typename T>
static BasicString<T> operator+(const BasicString<T>& lhs, const BasicString<T>& rhs)
{
	usize rlen = (lhs.Length() + rhs.Length()) * BasicString<T>::SPARE_RATE;
	BasicString<T> res(rlen);
	res = lhs;
	res.Append(rhs);
	return res;
}

template<typename T>
static BasicString<T> operator+(const BasicString<T>& lhs, const char* rhs)
{
	usize rlen = (lhs.Length() + strlen(rhs) + 1) * BasicString<T>::SPARE_RATE;
	BasicString<T> res(rlen);
	res = lhs;
	res.Append(rhs);
	return res;
}

template<typename T>
std::ostream& operator<<(std::ostream& os, const BasicString<T>& m) {
	return os << m.Buffer();
}

typedef BasicString<char> String;

TRE_NS_END