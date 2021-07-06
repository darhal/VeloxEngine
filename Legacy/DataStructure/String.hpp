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

#include <Legacy/Misc/Defines/Common.hpp>
#include <Legacy/Misc/Defines/Debug.hpp>
#include <Legacy/DataStructure/Vector.hpp>
#include <limits>
#include <type_traits>
#include <ostream>
#include <iterator>
#include <cstring>

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

	BasicString(usize capacity, usize len = 0);

	template<usize S>
	BasicString(const T(&str)[S], bool nop);
	// template<usize S>
	// BasicString(const T(&str)[S], usize capacity);

	BasicString(const char* str);

	~BasicString();

	// Copy ctor copy assignement
	BasicString(const BasicString<T>& other);
	BasicString<T>& operator=(const BasicString<T>& other);

	// Move ctor move assignement
	BasicString(BasicString<T>&& other);
	BasicString<T>& operator=(BasicString<T>&& other);

	INLINE void Reserve(usize s);
	INLINE void Clear();
	void Resize(usize s);
	
	void Append(const BasicString<T>& str);
	void Append(const char* str);
	void PushBack(T c);
	void PopBack();
	void Erase(usize pos, usize offset);
	void Insert(usize pos, const BasicString<T>& str);
	void Copy(const BasicString<T>& str, usize pos, usize offset);

	INLINE usize		   Length()							         ;
	INLINE const T*	   	   Buffer()							    const;
	INLINE usize		   Length()							    const;
	INLINE usize		   Size()								const;
	INLINE usize	       Capacity()							const;
	INLINE T			   At(usize i)							const;
	INLINE T			   Back()								const;
	INLINE T			   Front()								const;
	INLINE ssize		   Find(const BasicString<T>& pattren, uint32 start = 0)  const;
	INLINE BasicString<T>  SubString(usize pos, usize off)	    const;
	INLINE bool		   	   IsEmpty()							const;

	INLINE Vector<BasicString<T>> Split(T delimter);
	INLINE int32 FindLast(T delimter);
	INLINE int32 FindFirst(T delimter);
	INLINE void EraseAfterTheLast(T delimter);
	INLINE void EraseAfterTheFirst(T delimter);

	INLINE T* FindLastHelper(T* buffer, T delimter, usize length);
	INLINE T* FindFirstHelper(T* buffer, T delimter);

	INLINE Iterator begin() noexcept;
	INLINE Iterator end() noexcept;

	INLINE usize GetHash() const;

	template<typename NUMERIC_TYPE>
	T& operator[](const NUMERIC_TYPE i);
	template<typename NUMERIC_TYPE>
	const T& operator[](const NUMERIC_TYPE i) const;

	INLINE BasicString<T>& operator+=(const BasicString<T>& other);
	INLINE BasicString<T>& operator+=(const char* other);

	INLINE bool IsSmall() const;
public:
	CONSTEXPR static usize SPARE_RATE = 2;
private:
	CONSTEXPR static usize SSO_SIZE	  = (sizeof(T*) + 2 * sizeof(usize)) / sizeof(T); 
	CONSTEXPR static usize SSO_MI	  = SSO_SIZE - 1;  // MI stands for Max Index.
	
#if ENDIANNESS == LITTLE_ENDIAN
	CONSTEXPR static usize hight_bit_mask_normal_str = usize(1) << (sizeof(usize) * BITS_PER_BYTE - 1);
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

	INLINE void SetLength(usize nlen);
	INLINE void SetSmallLength(usize nlen);
	INLINE void SetNormalLength(usize nlen);
	INLINE void SetCapacity(usize ncap);
	INLINE T*   EditableBuffer();

	template<usize S, bool IsSmall>
	struct InitString {};

	template<usize S>
	struct InitString<S, true>
	{
		void operator()(BasicString<T>* obj, const T(&str)[S])
		{
			usize len = strlen(str) + 1;

			for (usize i = 0; i < len; i++) {
				obj->m_Data[i] = str[i];
			}

			obj->SetSmallLength(len);
		}
	};

	template<usize S>
	struct InitString<S, false>
	{
		void operator()(BasicString<T>* obj, const T(&str)[S])
		{
			usize len = strlen(str) + 1;
			usize real_cap = S * SPARE_RATE;
			obj->m_Buffer = (T*) operator new (sizeof(T)* real_cap); // allocate empty storage

			for (usize i = 0; i < len; i++) {
				obj->m_Buffer[i] = str[i];
			}

			obj->m_Capacity = real_cap;
			obj->SetNormalLength(len); // bit to indicate that its not small string 
									   // (the bit is located always at the end of the bit stream, 
								       // works both on little and big endian architectures)
		}
	};

	friend class File;
	friend class Directory;
};

template<typename T>
template<usize S>
BasicString<T>::BasicString(const T(&str)[S], bool nop)
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

/*template<typename T>
template<usize S>
BasicString<T>::BasicString(const T(&str)[S], usize capacity)
{
	if (capacity < S) 
		capacity = S;
		
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
}*/

template<typename T>
BasicString<T>::BasicString(const char* str)
{
	usize len = strlen(str) + 1; 
	usize S = len;

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
	ASSERTF((usize)i > (usize)m_Length, "Bad usage of [] with BasicString class, given index out of bounds.");
	return this->EditableBuffer()[i];
}

template<typename T>
template<typename NUMERIC_TYPE>
const T& BasicString<T>::operator[] (const NUMERIC_TYPE i) const
{
	ASSERTF((usize)i > (usize)m_Length, "Bad usage of [] with BasicString class, given index out of bounds.");
	return this->Buffer()[i];
}

// The preprocessing function for Boyer Moore's  
// bad character heuristic  
template<typename T, usize NB_CHAR>
void BadCharHeuristic(const BasicString<T>& str, ssize size, int32(&badchar)[NB_CHAR])
{
	// Initialize all occurrences as -1  
	for (usize i = 0; i < NB_CHAR; i++)
		badchar[i] = -1;

	// Fill the actual value of last occurrence  
	// of a character  
	for (ssize i = 0; i < size; i++)
		badchar[(uint32)str[i]] = int32(i);
}

/* A pattern searching function that uses Bad
Character Heuristic of Boyer Moore Algorithm */
template<typename T>
ssize SearchBoyerMoore(const BasicString<T>& txt, const BasicString<T>& pat, uint32 start = 0)
{
	CONSTEXPR usize NB_OF_CHARS = 2 * std::numeric_limits<uint16>::max() + 1;
	int32 badchar[NB_OF_CHARS];

	ssize m = (ssize)pat.Size();
	ssize n = (ssize)txt.Size() - start;
	const T* txt_buffer = txt.Buffer() + start;
	const T* pat_buffer = pat.Buffer();

	/* Fill the bad character array by calling
	the preprocessing function BadCharHeuristic()
	for given pattern */
	BadCharHeuristic(pat, m, badchar);

	ssize s = 0; // s is shift of the pattern with  
				// respect to text  
	while (s <= (n - m)) {
		ssize j = m - 1;

		/* Keep reducing index j of pattern while
		characters of pattern and text are
		matching at this shift s */
		while (j >= 0 && pat_buffer[j] == txt_buffer[s + j])
			j--;

		/* If the pattern is present at current
		shift, then index j will become -1 after
		the above loop */
		if (j < 0) {
			/* Shift the pattern so that the next
			character in text aligns with the last
			occurrence of it in pattern.
			The condition s+m < n is necessary for
			the case when pattern occurs at the end
			of text */
			return s; 
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
			s += MAX(1, j - badchar[static_cast<uint32>(txt_buffer[s + j])]);
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
	if (alen != b.Length()) {
		return false;
	}
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
	usize rlen = rhs.Length();

	if (rlen == 0)
		return lhs;

	rlen = (lhs.Length() + rlen) * BasicString<T>::SPARE_RATE;
	BasicString<T> res(rlen);
	res = lhs;
	res.Append(rhs);
	return res;
}

template<typename T>
static BasicString<T> operator+(const BasicString<T>& lhs, const char* rhs)
{
	if (rhs[0] == '\0')
		return lhs;

	usize rlen = (lhs.Length() + strlen(rhs) + 1) * BasicString<T>::SPARE_RATE;
	BasicString<T> res(rlen);
	res = lhs;
	res.Append(rhs);
	return res;
}

template<typename T>
usize BasicString<T>::GetHash() const
{
	const T* str = this->Buffer();
	usize hash = 5381;
	uint32 c;

	while ((c = *str++))
		hash = ((hash << 5) + hash) + c;

	return hash;
}

template<typename T>
static bool operator<(const BasicString<T>& lhs, const BasicString<T>& rhs)
{
	return strcmp(lhs.Buffer(), rhs.Buffer()) > 0;
}

template<typename T>
std::ostream& operator<<(std::ostream& os, const BasicString<T>& m) {
	return os << m.Buffer();
}

// #include "String.inl"

typedef BasicString<char> String;

TRE_NS_END

#include "String.inl"
