#pragma once

#include <Core/Misc/Defines/Common.hpp>
#include <Core/Utils/Memory.hpp>
#include <Core/Utils/Common.hpp>

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

    constexpr void PushBack(T ch);

    constexpr void PopBack();

    constexpr void Insert(usize pos, T ch);

    constexpr void Insert(usize pos, const T* str, usize sz = 0);

    constexpr void Insert(usize pos, const BasicString2<T>& str);

    constexpr void Erease(usize index, usize count);

    constexpr bool StartsWith(const T* str, usize sz = 0);

    constexpr bool StartsWith(const BasicString2<T>& str);

    constexpr bool EndsWith(const T* str, usize sz = 0);

    constexpr bool EndsWith(const BasicString2<T>& str);

    constexpr BasicString2<T> SubString(usize index, usize count);

    constexpr T Front() const { return m_Data[0]; }

    constexpr T Back() const { return m_Data[m_Size - 1]; }

    constexpr bool Reserve(usize size);

    constexpr usize Size() const { return m_Size; }

    constexpr bool IsSmall() const { return m_Data == m_Buffer; }

    constexpr usize Capacity() const { return this->IsSmall() ? NB_CHAR_SMALL : m_Capacity; }

    constexpr T* Data() { return m_Data; }

    constexpr T operator[](usize i) const { return m_Data[i]; }

    constexpr bool Empty() const { return m_Size == 0; }

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
    return this->Append(str.Data(), str.Size());
}

template<typename T>
constexpr void BasicString2<T>::PushBack(T ch)
{
    return this->Append(ch);
}

template<typename T>
constexpr void BasicString2<T>::PopBack()
{
    if (m_Size)
        m_Data[--m_Size] = NULL_TERMINATOR;
}

template<typename T>
constexpr void BasicString2<T>::Insert(usize pos, T ch)
{
    // TODO: assert pos is good and in range
    // ASSERTF(pos > m_Size, "Given index is out of bound please choose from [0..%" SZu "].", m_Size);

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
constexpr void BasicString2<T>::Insert(usize pos, const T* str, usize sz)
{
    // TODO: assert pos is good and in range

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
constexpr void BasicString2<T>::Insert(usize pos, const BasicString2<T>& str)
{
    return this->Insert(pos, str.Data(), str.Size());
}

template<typename T>
constexpr void BasicString2<T>::Erease(usize index, usize count)
{
    // TODO : Assert index and count are in range

    if (count > m_Size - index)
        count = m_Size - index;

    auto end = index + count;
    Utils::MoveConstruct(m_Data + index, m_Data + end, m_Size - end);
    m_Size -= count;
    m_Data[m_Size] = NULL_TERMINATOR;
}

template<typename T>
constexpr bool BasicString2<T>::StartsWith(const T* str, usize sz)
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
    return strncmp(m_Data, str, sz) == 0;
}

template<typename T>
constexpr bool BasicString2<T>::StartsWith(const BasicString2<T>& str)
{
    return this->StartsWith(str.Data(), str.Size());
}

template<typename T>
constexpr bool BasicString2<T>::EndsWith(const T* str, usize sz)
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
    return strncmp(m_Data + m_Size - sz, str, sz) == 0;
}

template<typename T>
constexpr bool BasicString2<T>::EndsWith(const BasicString2<T>& str)
{
    return this->StartsWith(str.Data(), str.Size());
}

template<typename T>
constexpr BasicString2<T> BasicString2<T>::SubString(usize index, usize count)
{
    if (count > m_Size - index)
        count = m_Size - index;
    return BasicString2(this->Data() + index, count);
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

    if (!this->IsSmall())
        Utils::FreeMemory(m_Data);

    m_Data = newData;
    m_Capacity = nCap;
}

using String2 = BasicString2<char>;

TRE_NS_END