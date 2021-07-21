#ifndef ARRAYVIEW_HPP
#define ARRAYVIEW_HPP

#include <Core/Misc/Defines/Common.hpp>
#include <Core/DataStructure/Utils.hpp>
#include <Core/Misc/UtilityConcepts.hpp>
#include <Core/DataStructure/RandomAccessIterator.hpp>

TRE_NS_START

template<typename T>
class ArrayView
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
    constexpr FORCEINLINE ArrayView() noexcept = default;

    constexpr FORCEINLINE ArrayView(T* data, usize size) noexcept;

    template<ContainerConcept ContainerType>
    constexpr FORCEINLINE ArrayView(const ContainerType& arr) noexcept;

    template<usize S>
    constexpr FORCEINLINE ArrayView(const T(&arr)[S]) noexcept;

    constexpr FORCEINLINE ~ArrayView() = default;

    constexpr FORCEINLINE ArrayView(const ArrayView& other) noexcept = default;

    constexpr FORCEINLINE ArrayView(ArrayView&& other) noexcept = default;

    constexpr FORCEINLINE ArrayView& operator=(const ArrayView& other) noexcept = default;

    constexpr FORCEINLINE ArrayView& operator=(ArrayView&& other) noexcept = default;

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

    constexpr FORCEINLINE ArrayView SubArray(usize start, usize end) const noexcept;

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
constexpr FORCEINLINE ArrayView<T>::ArrayView(T* data, usize size) noexcept
    : m_Data(data), m_Size(size)
{

}

template<typename T>
template<ContainerConcept ContainerType>
constexpr FORCEINLINE ArrayView<T>::ArrayView(const ContainerType& arr) noexcept
    : ArrayView(arr.Data(), arr.Size())
{

}

template<typename T>
template<usize S>
constexpr FORCEINLINE ArrayView<T>::ArrayView(const T(&arr)[S]) noexcept
    : ArrayView(arr, S)
{

}

template<typename T>
constexpr FORCEINLINE ArrayView<T> ArrayView<T>::SubArray(usize index, usize count) const noexcept
{
    if (count > m_Size - index)
        count = m_Size - index;
    return ArrayView(m_Data + index, count);
}

TRE_NS_END

#endif // ARRAYVIEW_HPP
