#pragma once

#include <string.h>

#include <Core/Misc/Defines/Common.hpp>
#include <Core/Memory/Memory.hpp>
#include <Core/Misc/Defines/Debug.hpp>
#include <Core/DataStructure/RandomAccessIterator.hpp>
#include <Core/DataStructure/Utils.hpp>

TRE_NS_START

template<typename T>
class BasicString
{
public:
    using Iterator          = RandomAccessIterator<T>;
    using CIterator         = RandomAccessIterator<const T>;
    using value_type        = T;
    using pointer           = value_type*;
    using const_pointer     = const value_type*;
    using reference         = value_type&;
    using const_reference	= const value_type&;
    using const_iterator	= CIterator;
    using iterator          = const_iterator;
    using const_reverse_iterator = std::reverse_iterator<const_iterator>;
    using reverse_iterator  = const_reverse_iterator;
    using size_type         = size_t;
    using difference_type	= ptrdiff_t;

public:
    constexpr FORCEINLINE BasicString();

    constexpr BasicString(const T* data, usize size);

    constexpr FORCEINLINE BasicString(const std::basic_string<T>& str);

    template<usize S>
    constexpr FORCEINLINE BasicString(const T(&str)[S]);

    constexpr FORCEINLINE ~BasicString() noexcept;

    constexpr FORCEINLINE BasicString(const BasicString<T>& other);

    constexpr FORCEINLINE BasicString& operator=(const BasicString<T>& other);

    constexpr FORCEINLINE BasicString(BasicString<T>&& other) noexcept;

    constexpr FORCEINLINE BasicString& operator=(BasicString<T>&& other) noexcept;

    constexpr FORCEINLINE void Append(T ch);

    constexpr void Append(const T* str, usize sz = 0);

    constexpr FORCEINLINE void Append(const BasicString<T>& str);

    constexpr void PushBack(T ch);

    constexpr FORCEINLINE void PopBack() noexcept;

    constexpr void Insert(usize pos, T ch);

    constexpr void Insert(usize pos, const T* str, usize sz = 0);

    constexpr FORCEINLINE void Insert(usize pos, const BasicString<T>& str);

    constexpr void Erase(usize index, usize count) noexcept;

    constexpr bool StartsWith(const T* str, usize sz = 0) const noexcept;

    constexpr bool StartsWith(const BasicString<T>& str) const noexcept;

    constexpr bool EndsWith(const T* str, usize sz = 0) const noexcept;

    constexpr bool EndsWith(const BasicString<T>& str) const noexcept;

    constexpr bool Reserve(usize size);

    constexpr FORCEINLINE BasicString<T> SubString(usize index, usize count) const;

    constexpr FORCEINLINE T Front() const noexcept { return m_Data[0]; }

    constexpr FORCEINLINE T Back() const noexcept { return m_Data[m_Size - 1]; }

    constexpr FORCEINLINE usize Size() const noexcept{ return m_Size; }

    constexpr FORCEINLINE bool IsSmall() const noexcept { return m_Data == m_Buffer; }

    constexpr FORCEINLINE usize Capacity() const noexcept { return this->IsSmall() ? NB_CHAR_SMALL : m_Capacity; }

    constexpr FORCEINLINE T* Data() noexcept { return m_Data; }

    constexpr FORCEINLINE const T* Data() const noexcept { return m_Data; }

    constexpr FORCEINLINE T operator[](usize i) const noexcept { return m_Data[i]; }

    constexpr FORCEINLINE T& operator[](usize i) noexcept { return m_Data[i]; }

    constexpr FORCEINLINE bool Empty() const noexcept { return m_Size == 0; }

    constexpr iterator begin() const noexcept
    {
        return Iterator(m_Data);
    }

    constexpr iterator end() const noexcept
    {
        return Iterator(m_Data + m_Size);
    }

    constexpr const_iterator cbegin() const noexcept
    {
        return CIterator(m_Data);
    }

    constexpr const_iterator cend() const noexcept
    {
        return CIterator(m_Data + m_Size);
    }

    constexpr const_reverse_iterator rbegin() const noexcept
    {
        return const_reverse_iterator(this->end());
    }

    constexpr const_reverse_iterator rend() const noexcept
    {
        return const_reverse_iterator(this->begin());
    }

    constexpr const_reverse_iterator crbegin() const noexcept
    {
        return const_reverse_iterator(this->end());
    }

    constexpr const_reverse_iterator  crend() const noexcept
    {
        return const_reverse_iterator(this->begin());
    }

    constexpr friend void Swap(BasicString<T>& first, BasicString<T>& second) noexcept
    {
        if (first.IsSmall() && second.IsSmall()) {
            T buffer[NB_ELEMENTS];
            Utils::Copy(buffer, first.m_Data, NB_ELEMENTS);
            Utils::Copy(first.m_Data, second.m_Data, NB_ELEMENTS);
            Utils::Copy(second.m_Data, buffer, NB_ELEMENTS);
            std::swap(first.m_Size, second.m_Size);
        }else if ((first.IsSmall() && !second.IsSmall()) || (second.IsSmall() && !first.IsSmall())) {
            BasicString<T>* small = first.IsSmall() ? &first : &second;
            BasicString<T>* notSmall = small == &first ? &second : &first;
            auto notSmallCap = notSmall->m_Capacity;
            Utils::Copy(notSmall->m_Buffer, small->m_Data, small->m_Size);
            small->m_Data = notSmall->m_Data;
            small->m_Capacity = notSmallCap;
            notSmall->m_Data = notSmall->m_Buffer;
            std::swap(small->m_Size, notSmall->m_Size);
        }else{
            std::swap(first.m_Data, second.m_Data);
            std::swap(first.m_Capacity, second.m_Capacity);
            std::swap(first.m_Size, second.m_Size);
        }
    }

    constexpr FORCEINLINE friend void swap(BasicString<T>& first, BasicString<T>& second) noexcept
    {
        Swap(first, second);
    }

private:
    constexpr void Reallocate(usize nCap);

private:
    constexpr static const auto SSO_SIZE        = 2 * sizeof(usize);
    constexpr static const auto NB_CHAR_SMALL   = SSO_SIZE / sizeof(T) - 1;
    constexpr static const auto NB_ELEMENTS     = NB_CHAR_SMALL + 1;
    constexpr static const T NULL_TERMINATOR    = '\0';
    constexpr static const T DEFAULT_GROW_SIZE  = 2;

private:
	T* m_Data;
    usize m_Size;

    union {
        T m_Buffer[NB_ELEMENTS];
        usize m_Capacity;
    };
};

template<typename T>
constexpr FORCEINLINE BasicString<T>::BasicString() : m_Data(m_Buffer), m_Size(0), m_Buffer{}
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
constexpr FORCEINLINE BasicString<T>::BasicString(const std::basic_string<T>& str)
    : BasicString(str.c_str(), str.size())
{

}

template<typename T>
template<usize S>
constexpr FORCEINLINE BasicString<T>::BasicString(const T(&str)[S])
    : BasicString(str, S - 1)
{

}

template<typename T>
constexpr FORCEINLINE BasicString<T>::~BasicString() noexcept
{
    if (m_Data && !this->IsSmall()) {
        Utils::FreeMemory(m_Data);
    }
}

template<typename T>
constexpr FORCEINLINE void BasicString<T>::Append(T ch)
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
constexpr FORCEINLINE void BasicString<T>::Append(const BasicString<T>& str)
{
    return this->Append(str.Data(), str.Size());
}

template<typename T>
constexpr FORCEINLINE void BasicString<T>::PushBack(T ch)
{
    return this->Append(ch);
}

template<typename T>
constexpr FORCEINLINE void BasicString<T>::PopBack() noexcept
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
constexpr FORCEINLINE void BasicString<T>::Insert(usize pos, const BasicString<T>& str)
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
constexpr bool BasicString<T>::StartsWith(const T* str, usize sz) const noexcept
{
    if (str != nullptr && sz == 0)
        sz = Utils::Strlen(str);

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
constexpr FORCEINLINE bool BasicString<T>::StartsWith(const BasicString<T>& str) const noexcept
{
    return this->StartsWith(str.Data(), str.Size());
}

template<typename T>
constexpr bool BasicString<T>::EndsWith(const T* str, usize sz) const noexcept
{
    if (str != nullptr && sz == 0)
        sz = Utils::Strlen(str);

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
constexpr FORCEINLINE bool BasicString<T>::EndsWith(const BasicString<T>& str) const noexcept
{
    return this->StartsWith(str.Data(), str.Size());
}

template<typename T>
constexpr FORCEINLINE BasicString<T> BasicString<T>::SubString(usize index, usize count) const
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

template<typename T>
constexpr FORCEINLINE BasicString<T>::BasicString(const BasicString<T>& other)
    : m_Size(other.m_Size)
{
    if (other.IsSmall()) {
        Utils::Copy(m_Buffer, other.m_Buffer, m_Size);
        m_Data = m_Buffer;
    }else{
        m_Capacity = other.m_Capacity;
        m_Data = Utils::Allocate<T>(m_Capacity);
        Utils::Copy(m_Data, other.m_Data, m_Size);
    }
}

template<typename T>
constexpr FORCEINLINE BasicString<T>& BasicString<T>::operator=(const BasicString<T>& other)
{
    BasicString<T> tmp(other);
    Swap(*this, tmp);
    return *this;
}

template<typename T>
constexpr FORCEINLINE BasicString<T>::BasicString(BasicString<T>&& other) noexcept
    : m_Size(other.m_Size)
{
    if (other.IsSmall()) {
        Utils::Copy(m_Buffer, other.m_Buffer, m_Size);
        m_Data = m_Buffer;
    }else{
        m_Capacity = other.m_Capacity;
        m_Data = other.m_Data;
        other.m_Data = nullptr;
    }
}

template<typename T>
constexpr FORCEINLINE BasicString<T>& BasicString<T>::operator=(BasicString<T>&& other) noexcept
{
    BasicString<T> tmp(std::move(other));
    Swap(*this, tmp);
    return *this;
}

using String = BasicString<char>;

TRE_NS_END
