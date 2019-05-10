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
	void Copy(BasicString<T>& str, usize pos, usize offset);
	/*template<usize S>
	void Insert(usize pos, const T(&str)[S]);
	template<usize S>
	void Insert(usize pos, const char* str, usize str_len);*/

	FORCEINLINE const T*	Buffer()	const;
	FORCEINLINE const usize	Length()	const;
	FORCEINLINE const usize	Size()		const;
	FORCEINLINE const usize	Capacity()	const;
	FORCEINLINE T			At(usize i) const;
	FORCEINLINE T			Back()		const;
	FORCEINLINE T			Front()		const;
	FORCEINLINE ssize		Find(const BasicString<T>& pattren) const;

	template<typename NUMERIC_TYPE>
	T& operator[](const NUMERIC_TYPE i);

	template<typename NUMERIC_TYPE>
	const T& operator[](const NUMERIC_TYPE i) const;
private:
public:
	typedef BasicString<T> CLASS_TYPE;

	CONSTEXPR static usize SSO_SIZE = (sizeof(T*) + 2 * sizeof(usize)) / sizeof(T) - 1; // remove the one because we start at 0.

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
};


template<typename T>
template<usize S>
BasicString<T>::BasicString(const T(&str)[S])
{
	if (S <= SSO_SIZE) { // here its <= because we will count the trailing null
		for (usize i = 0; i < S; i++) {
			m_Data[i] = str[i];
		}
		m_Data[SSO_SIZE] = T((SSO_SIZE - S) << 1);
	}else{
		m_Buffer = (T*) operator new (sizeof(T)*S); // allocate empty storage
		for (usize i = 0; i < S; i++) {
			m_Buffer[i] = str[i];
		}
		m_Capacity = S;
		m_Length = S << 1 | 0x01;
						     //^ bit to indicate that its not small string
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
		int32 j = m - 1;

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
			return s;
			s += (s + m < n) ? m - badchar[txt_buffer[s + m]] : 1;

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

typedef BasicString<char> String;

TRE_NS_END