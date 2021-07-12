#ifndef STRINGVIEW_HPP
#define STRINGVIEW_HPP

#include <Core/DataStructure/String.hpp>
#include <Core/DataStructure/Utils.hpp>
#include <Core/DataStructure/RandomAccessIterator.hpp>

TRE_NS_START

std::string_view a;

template<typename T>
class BasicStringView
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
    constexpr FORCEINLINE BasicStringView() noexcept = default;

    constexpr FORCEINLINE BasicStringView(T* data, usize size) noexcept;

    constexpr FORCEINLINE BasicStringView(const BasicString<T>& str) noexcept;

    template<usize S>
    constexpr FORCEINLINE BasicStringView(const T(&str)[S]) noexcept;

    constexpr FORCEINLINE ~BasicStringView() = default;

    constexpr FORCEINLINE BasicStringView(const BasicStringView& other) noexcept = default;

    constexpr FORCEINLINE BasicStringView(BasicStringView&& other) noexcept = default;

    constexpr FORCEINLINE BasicStringView& operator=(const BasicStringView& other) noexcept = default;

    constexpr FORCEINLINE BasicStringView& operator=(BasicStringView&& other) noexcept = default;

    constexpr FORCEINLINE const T* Data() const { return m_Data; }

    constexpr FORCEINLINE const T* Length() const { return m_Size; }

    constexpr FORCEINLINE const T* Size() const { return m_Size; }

    constexpr FORCEINLINE T operator[](usize idx) { return m_Data[idx]; }

    constexpr FORCEINLINE const T& operator[](usize idx) const { return m_Data[idx]; }

    constexpr FORCEINLINE T At(usize idx) const { return m_Data[idx]; }

    constexpr FORCEINLINE T Front() const { return *m_Data; }

    constexpr FORCEINLINE T Back() const { return m_Data[m_Size - 1]; }

    constexpr FORCEINLINE void RemovePrefix(usize skip) { m_Data += skip; }

    constexpr FORCEINLINE void RemoveSuffix(usize skip) { m_Size -= skip; }

    constexpr FORCEINLINE BasicStringView SubString(usize start, usize end) const noexcept;

    constexpr FORCEINLINE bool StartsWith(const T* str, usize sz = 0) const noexcept;

    constexpr FORCEINLINE bool StartsWith(const BasicStringView& str) const noexcept;

    constexpr FORCEINLINE bool EndsWith(const T* str, usize sz = 0) const noexcept;

    constexpr FORCEINLINE bool EndsWith(const BasicStringView& str) const noexcept;

    constexpr FORCEINLINE usize Find(const T* str, usize sz = 0) const noexcept;

    constexpr FORCEINLINE usize Find(const BasicStringView& str) const noexcept;

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
private:
    T* m_Data;
    usize m_Size;
};

template<typename T>
constexpr FORCEINLINE BasicStringView<T>::BasicStringView(T* data, usize size) noexcept
    : m_Data(data), m_Size(size)
{

}

template<typename T>
constexpr FORCEINLINE BasicStringView<T>::BasicStringView(const BasicString<T>& str) noexcept
    : BasicStringView(str.Data(), str.Size())
{

}

template<typename T>
template<usize S>
constexpr FORCEINLINE BasicStringView<T>::BasicStringView(const T(&str)[S]) noexcept
    : BasicStringView(str, S - 1)
{

}

template<typename T>
constexpr FORCEINLINE BasicStringView<T> BasicStringView<T>::SubString(usize index, usize count) const noexcept
{
    if (count > m_Size - index)
        count = m_Size - index;
    return BasicStringView(m_Data + index, count);
}

template<typename T>
constexpr FORCEINLINE bool BasicStringView<T>::StartsWith(const T* str, usize sz) const noexcept
{
    if (str != nullptr && sz == 0)
        sz = Utils::Strlen(str);

    if (sz > m_Size)
        return false;

    return Utils::MemCmp(m_Data, str, sz);
}

template<typename T>
constexpr FORCEINLINE bool BasicStringView<T>::StartsWith(const BasicStringView& str) const noexcept
{
    return this->StartsWith(str.m_Data, str.m_Size);
}

template<typename T>
constexpr FORCEINLINE bool BasicStringView<T>::EndsWith(const T* str, usize sz) const noexcept
{
    if (str != nullptr && sz == 0)
        sz = Utils::Strlen(str);

    if (sz > m_Size)
        return false;

    return Utils::MemCmp(m_Data + m_Size - sz, str, sz);
}

template<typename T>
constexpr FORCEINLINE bool BasicStringView<T>::EndsWith(const BasicStringView& str) const noexcept
{
    return this->EndsWith(str.m_Data, str.m_Size);
}

template<typename T>
constexpr FORCEINLINE usize BasicStringView<T>::Find(const T* str, usize sz) const noexcept
{
    // TODO : Implement this
    return std::numeric_limits<usize>::max();
}

template<typename T>
constexpr FORCEINLINE usize BasicStringView<T>::Find(const BasicStringView& str) const noexcept
{
    return this->Find(str.m_Data, str.m_Size);
}

using StringView = BasicStringView<char>;


TRE_NS_END

#endif // STRINGVIEW_HPP
