#pragma once

#include <string.h>

#include <Core/Misc/Defines/Common.hpp>
#include <Core/Memory/Memory.hpp>
#include <Core/Misc/Defines/Debug.hpp>

TRE_NS_START

template<typename T>
class BasicString
{
public:
    FORCEINLINE constexpr BasicString();

    constexpr BasicString(const T* data, usize size);

    FORCEINLINE constexpr BasicString(const std::basic_string<T>& str);

    template<usize S>
    FORCEINLINE constexpr BasicString(const T(&str)[S]);

    FORCEINLINE constexpr ~BasicString() noexcept;

    FORCEINLINE constexpr void Append(T ch);

    constexpr void Append(const T* str, usize sz = 0);

    FORCEINLINE constexpr void Append(const BasicString<T>& str);

    constexpr void PushBack(T ch);

    FORCEINLINE constexpr void PopBack() noexcept;

    constexpr void Insert(usize pos, T ch);

    constexpr void Insert(usize pos, const T* str, usize sz = 0);

    FORCEINLINE constexpr void Insert(usize pos, const BasicString<T>& str);

    constexpr void Erase(usize index, usize count) noexcept;

    constexpr bool StartsWith(const T* str, usize sz = 0) noexcept;

    constexpr bool StartsWith(const BasicString<T>& str) noexcept;

    constexpr bool EndsWith(const T* str, usize sz = 0) noexcept;

    constexpr bool EndsWith(const BasicString<T>& str) noexcept;

    constexpr bool Reserve(usize size);

    FORCEINLINE constexpr BasicString<T> SubString(usize index, usize count);

    FORCEINLINE constexpr T Front() const noexcept { return m_Data[0]; }

    FORCEINLINE constexpr T Back() const noexcept { return m_Data[m_Size - 1]; }

    FORCEINLINE constexpr usize Size() const noexcept{ return m_Size; }

    FORCEINLINE constexpr bool IsSmall() const noexcept { return m_Data == m_Buffer; }

    FORCEINLINE constexpr usize Capacity() const noexcept { return this->IsSmall() ? NB_CHAR_SMALL : m_Capacity; }

    FORCEINLINE constexpr T* Data() noexcept { return m_Data; }

    FORCEINLINE constexpr const T* Data() const noexcept { return m_Data; }

    FORCEINLINE constexpr T operator[](usize i) const noexcept { return m_Data[i]; }

    FORCEINLINE constexpr bool Empty() const noexcept { return m_Size == 0; }

private:
    constexpr void Reallocate(usize nCap);

private:
    CONSTEXPR static const auto SSO_SIZE        = 2 * sizeof(usize);
    CONSTEXPR static const auto NB_CHAR_SMALL   = SSO_SIZE / sizeof(T) - 1;
    CONSTEXPR static const T NULL_TERMINATOR    = '\0';
    CONSTEXPR static const T DEFAULT_GROW_SIZE  = 2;

private:
	T* m_Data;
    usize m_Size;

    union {
        T m_Buffer[NB_CHAR_SMALL + 1];
        usize m_Capacity;
    };
};

template<typename T>
FORCEINLINE constexpr BasicString<T>::BasicString() : m_Data(m_Buffer), m_Size(0), m_Buffer{}
{

}

template<typename T>
constexpr BasicString<T>::BasicString(const T* data, usize size)
    : m_Size(size)
{
    if (m_Size < NB_CHAR_SMALL) {
        m_Data = m_Buffer;
    }else{
        m_Data = Utils::Allocate<T>(m_Size + 1);
        m_Capacity = m_Size;
    }

    Utils::Copy(m_Data, data, size + 1);
}

template<typename T>
FORCEINLINE constexpr BasicString<T>::BasicString(const std::basic_string<T>& str)
    : BasicString(str.c_str(), str.size())
{

}

template<typename T>
template<usize S>
FORCEINLINE constexpr BasicString<T>::BasicString(const T(&str)[S])
    : BasicString(str, S - 1)
{

}

template<typename T>
FORCEINLINE constexpr BasicString<T>::~BasicString() noexcept
{
    if (!this->IsSmall()) {
        Utils::FreeMemory(m_Data);
    }
}

template<typename T>
FORCEINLINE constexpr void BasicString<T>::Append(T ch)
{
    this->Reserve(m_Size + 2);
    m_Data[m_Size++] = ch;
    m_Data[m_Size] = NULL_TERMINATOR;
}

template<typename T>
constexpr void BasicString<T>::Append(const T* str, usize sz)
{
    if (str != nullptr && sz == 0)
        sz = strlen(str);

    this->Reserve(m_Size + sz + 1);

    for (usize i = 0; i < sz; i++) {
        m_Data[m_Size++] = str[i];
    }

    m_Data[m_Size] = NULL_TERMINATOR;
}

template<typename T>
FORCEINLINE constexpr void BasicString<T>::Append(const BasicString<T>& str)
{
    return this->Append(str.Data(), str.Size());
}

template<typename T>
FORCEINLINE constexpr void BasicString<T>::PushBack(T ch)
{
    return this->Append(ch);
}

template<typename T>
FORCEINLINE constexpr void BasicString<T>::PopBack() noexcept
{
    if (m_Size)
        m_Data[--m_Size] = NULL_TERMINATOR;
}

template<typename T>
constexpr void BasicString<T>::Insert(usize pos, T ch)
{
    TRE_ASSERTF(pos <= m_Size, "Given index is out of bound. Index must be in  [0..%" SZu "[", m_Size);

    usize cap = this->Capacity();

    if (m_Size + 2 >= cap) {
        usize nCap = cap * DEFAULT_GROW_SIZE;
        T* newData = Utils::Allocate<T>(nCap);
        Utils::Copy(newData, m_Data, pos);
        newData[pos] = ch;
        Utils::Copy(newData + pos + 1, m_Data + pos, m_Size - pos);

        if (!this->IsSmall())
            Utils::FreeMemory(m_Data);

        m_Data = newData;
        m_Capacity = nCap;
        m_Data[++m_Size] = NULL_TERMINATOR;
    } else {
        // shift all of this to keep place for the new element
        Utils::MoveConstructBackward(m_Data + 1, m_Data, m_Size - 1, pos);
        m_Data[pos] = ch;
        m_Data[++m_Size] = NULL_TERMINATOR;
    }

    // return this->Insert(pos, &ch, 1);
}

template<typename T>
constexpr void BasicString<T>::Insert(usize pos, const T* str, usize sz)
{
   TRE_ASSERTF(pos <= m_Size, "Given index is out of bound. Index must be in  [0..%" SZu "[", m_Size);

    if (str != nullptr && sz == 0)
        sz = strlen(str);

    usize cap = this->Capacity();

    if (m_Size + sz + 1 >= cap) {
        usize nCap = (m_Size + sz) * DEFAULT_GROW_SIZE;
        T* newData = Utils::Allocate<T>(nCap);
        Utils::Copy(newData, m_Data, pos);
        Utils::Copy(newData + pos, str, sz);
        Utils::Copy(newData + pos + sz, m_Data + pos, m_Size - pos);

        if (!this->IsSmall())
            Utils::FreeMemory(m_Data);

        m_Data = newData;
        m_Capacity = nCap;
        m_Size += sz;
        m_Data[m_Size] = NULL_TERMINATOR;
    } else {
        // shift all of this to keep place for the new element
        Utils::MoveConstructBackward(m_Data + sz, m_Data, m_Size - 1, pos);
        Utils::Copy(m_Data + pos, str, sz);
        m_Size += sz;
        m_Data[m_Size] = NULL_TERMINATOR;
    }
}

template<typename T>
FORCEINLINE constexpr void BasicString<T>::Insert(usize pos, const BasicString<T>& str)
{
    return this->Insert(pos, str.Data(), str.Size());
}

template<typename T>
constexpr void BasicString<T>::Erase(usize index, usize count) noexcept
{
    TRE_ASSERTF(index < m_Size, "Given index is out of bound. Index must be in  [0..%" SZu "[", m_Size);

    if (!count)
        return;

    if (count > m_Size - index)
        count = m_Size - index;

    auto end = index + count;
    Utils::MoveConstruct(m_Data + index, m_Data + end, m_Size - end);
    m_Size -= count;
    m_Data[m_Size] = NULL_TERMINATOR;
}

template<typename T>
constexpr bool BasicString<T>::StartsWith(const T* str, usize sz) noexcept
{
    if (str != nullptr && sz == 0)
        sz = strlen(str);

    if (sz > m_Size)
        return false;

    /*for (usize i = 0; i < sz; i++) {
        if (m_Data[i] != str[i])
            return false;
    }*/

    // return true;
    return Utils::MemCmp(m_Data, str, sz);
}

template<typename T>
FORCEINLINE constexpr bool BasicString<T>::StartsWith(const BasicString<T>& str) noexcept
{
    return this->StartsWith(str.Data(), str.Size());
}

template<typename T>
constexpr bool BasicString<T>::EndsWith(const T* str, usize sz) noexcept
{
    if (str != nullptr && sz == 0)
        sz = strlen(str);

    if (sz > m_Size)
        return false;

    /*for (usize i = 0; i < sz; i++) {
        if (m_Data[m_Size - sz + i - 1] != str[i])
            return false;
    }*/

    // return true;
    return Utils::MemCmp(m_Data + m_Size - sz, str, sz);
}

template<typename T>
FORCEINLINE constexpr bool BasicString<T>::EndsWith(const BasicString<T>& str) noexcept
{
    return this->StartsWith(str.Data(), str.Size());
}

template<typename T>
FORCEINLINE constexpr BasicString<T> BasicString<T>::SubString(usize index, usize count)
{
    if (count > m_Size - index)
        count = m_Size - index;
    return BasicString(this->Data() + index, count);
}

template<typename T>
constexpr bool BasicString<T>::Reserve(usize size)
{
    usize cap = this->Capacity();

    if (size < cap)
        return false;

    size = size * DEFAULT_GROW_SIZE;
    this->Reallocate(size);
    return true;
}

template<typename T>
constexpr void BasicString<T>::Reallocate(usize nCap)
{
    T* newData = Utils::Allocate<T>(nCap);
    Utils::Copy(newData, m_Data, m_Size + 1);

    if (!this->IsSmall())
        Utils::FreeMemory(m_Data);

    m_Data = newData;
    m_Capacity = nCap;
}

using String = BasicString<char>;

TRE_NS_END
