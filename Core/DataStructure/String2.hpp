#pragma once

#include <Core/Misc/Defines/Common.hpp>
#include <Core/Utils/Memory.hpp>

TRE_NS_START

template<typename T>
class BasicString2
{
public:
    constexpr BasicString2();

    constexpr BasicString2(const T* data, usize size);

    template<usize S>
    constexpr BasicString2(const T(&str)[S]);

    constexpr ~BasicString2();

    constexpr void Append(T ch);

    constexpr void Append(const T* str, usize sz = 0);

    constexpr void Append(const BasicString2<T>& str);

    constexpr bool Reserve(usize size);

    constexpr usize Size() const { return m_Size; }

    constexpr bool IsSmall() const { return m_Data == m_Buffer; }

    constexpr usize Capacity() const { return this->IsSmall() ? NB_CHAR_SMALL : m_Capacity; }

    constexpr T* Data() { return m_Data; }

    constexpr T operator[](usize i) const { return m_Data[i]; }

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
constexpr BasicString2<T>::BasicString2() : m_Data(m_Buffer), m_Size(0), m_Buffer{}
{

}

template<typename T>
constexpr BasicString2<T>::BasicString2(const T* data, usize size)
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
template<usize S>
constexpr BasicString2<T>::BasicString2(const T(&str)[S])
    : BasicString2(str, S - 1)
{

}

template<typename T>
constexpr BasicString2<T>::~BasicString2()
{
    if (!this->IsSmall()) {
        Utils::FreeMemory(m_Data);
    }
}

template<typename T>
constexpr void BasicString2<T>::Append(T ch)
{
    this->Reserve(m_Size + 2);
    m_Data[m_Size++] = ch;
    m_Data[m_Size] = NULL_TERMINATOR;
}

template<typename T>
constexpr void BasicString2<T>::Append(const T* str, usize sz)
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
constexpr void BasicString2<T>::Append(const BasicString2<T>& str)
{
    this->Append(str.Data(), str.Size());
}

template<typename T>
constexpr bool BasicString2<T>::Reserve(usize size)
{
    usize cap = this->Capacity();

    if (size < cap)
        return false;

    size = size * DEFAULT_GROW_SIZE;
    this->Reallocate(size);
    return true;
}

template<typename T>
constexpr void BasicString2<T>::Reallocate(usize nCap)
{
    T* newData = Utils::Allocate<T>(nCap);
    Utils::Copy(newData, m_Data, m_Size + 1);

    if (!this->IsSmall()) {
        Utils::FreeMemory(m_Data);
    }

    m_Data = newData;
    m_Capacity = nCap;
}

using String2 = BasicString2<char>;

TRE_NS_END
