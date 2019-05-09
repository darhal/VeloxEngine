#pragma once

#include <Core/Misc/Defines/Common.hpp>
#include <Core/Misc/Defines/Debug.hpp>
#include <Core/Misc/Maths/Utils.hpp>
#include <limits>


TRE_NS_START

template<typename T>
class BasicString
{
public:
	BasicString();

	BasicString(usize capacity);

	template<usize S>
	BasicString(const T(&str)[S]);

	~BasicString();

	FORCEINLINE bool Empty();
	FORCEINLINE void Reserve(usize s);
	FORCEINLINE void Resize(usize s);
	FORCEINLINE void Clear();
	

	void Append(const BasicString<T>& str);
	void PushBack(T c);
	void Erase(usize pos, usize offset);
	void Insert(usize pos, const BasicString<T>& str);
	void Copy(const BasicString<T>& str, usize pos, usize offset);
	/*template<usize S>
	void Insert(usize pos, const T(&str)[S]);
	template<usize S>
	void Insert(usize pos, const char* str, usize str_len);*/

	FORCEINLINE const T*	Buffer()	const;
	FORCEINLINE const usize	Length()	const;
	FORCEINLINE const usize	Capacity()	const;
	FORCEINLINE T			At(usize i) const;
	FORCEINLINE T			Back()		const;
	FORCEINLINE T			Front()		const;
	FORCEINLINE ssize		Find(const BasicString<T>& pattren) const;

	template<typename NUMERIC_TYPE>
	T& operator[](const NUMERIC_TYPE i);

	template<typename NUMERIC_TYPE>
	const T& operator[](const NUMERIC_TYPE i) const;

	template<typename U>
	friend bool operator== (const BasicString<U>& a, const BasicString<U>& b);
	template<typename U>
	friend bool operator!= (const BasicString<U>& a, const BasicString<U>& b);
	/*template<typename U>
	friend bool operator== (const BasicString<U>& a, const char* b);
	template<typename U>
	friend bool operator== (const char* b, const BasicString<U>& a);

	template<typename U>
	friend bool operator== (const BasicString<U>& a, const char* b);
	template<typename U>
	friend bool operator== (const char* b, const BasicString<U>& a);*/
private:
	T*		m_Buffer;
	usize	m_Length;
	usize	m_Capacity;
};


template<typename T>
template<usize S>
BasicString<T>::BasicString(const T(&str)[S]) : m_Capacity(S), m_Length(S-1)
{
	m_Buffer = (T*) operator new (sizeof(T)*S); // allocate empty storage
	for (usize i = 0; i < S; i++) {
		m_Buffer[i] = str[i];
	}
}

template<typename T>
template<typename NUMERIC_TYPE>
T& BasicString<T>::operator[] (const NUMERIC_TYPE i)
{
	ASSERTF(!(Absolute(i) > (ssize)m_Length), "Bad usage of [] with BasicString class, given index out of bounds.");
	return m_Buffer[i];
}

template<typename T>
template<typename NUMERIC_TYPE>
const T& BasicString<T>::operator[] (const NUMERIC_TYPE i) const
{
	ASSERTF(!(Absolute(i) > (ssize)m_Length), "Bad usage of [] with BasicString class, given index out of bounds.");
	return m_Buffer[i];
}

template<typename U>
static bool operator==(BasicString<U>&& a, BasicString<U>&& b)
{
	if (a.m_Length != b.m_Length) return false;
	for (usize i = 0; i < a.m_Length; i++) {
		if (a.m_Buffer[i] != b.m_Buffer[i]) {
			return false;
		}
	}
	return true;
}

template<typename U>
static bool operator!=(BasicString<U>&& a, BasicString<U>&& b)
{
	return !(a == b);
}

template<typename U>
static bool operator==(const BasicString<U>& a, const BasicString<U>& b)
{
	if (a.m_Length != b.m_Length) return false;
	for (usize i = 0; i < a.m_Length; i++) {
		if (a.m_Buffer[i] != b.m_Buffer[i]) {
			return false;
		}
	}
	return true;
}

template<typename U>
static bool operator!=(const BasicString<U>& a, const BasicString<U>& b)
{
	return !(a == b);
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
	for (i = 0; i < size; i++)
		badchar[(int)str[i]] = i;
}

/* A pattern searching function that uses Bad
Character Heuristic of Boyer Moore Algorithm */
template<typename T>
ssize SearchBoyerMoore(const BasicString<T>& txt, const BasicString<T>& pat)
{
	ssize m = (ssize)pat.Length();
	ssize n = (ssize)txt.Length();

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
		int32 j = m - 1;

		/* Keep reducing index j of pattern while
		characters of pattern and text are
		matching at this shift s */
		while (j >= 0 && pat[j] == txt[s + j])
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
			return s;
			s += (s + m < n) ? m - badchar[txt[s + m]] : 1;

		} else {
			/* Shift the pattern so that the bad character
			in text aligns with the last occurrence of
			it in pattern. The max function is used to
			make sure that we get a positive shift.
			We may get a negative shift if the last
			occurrence of bad character in pattern
			is on the right side of the current
			character. */
			s += MAX(1, j - badchar[txt[s + j]]);
		}
	}
	return -1;
}

typedef BasicString<char> String;

TRE_NS_END