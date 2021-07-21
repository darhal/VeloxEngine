#ifndef VECTORBASE_HPP
#define VECTORBASE_HPP

#include <initializer_list>
#include <iterator>
#include <utility>

#include <Core/Misc/Defines/Common.hpp>
#include <Core/Misc/Defines/Debug.hpp>
#include <Core/Misc/UtilityConcepts.hpp>
#include <Core/Memory/Memory.hpp>
#include <Core/Memory/GenericAllocator.hpp>
#include <Core/DataStructure/RandomAccessIterator.hpp>

TRE_NS_START

template<typename T>
class VectorBase
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
    using iterator          = Iterator;
    using const_reverse_iterator = std::reverse_iterator<const_iterator>;
    using reverse_iterator  = const_reverse_iterator;
    using size_type         = size_t;
    using difference_type	= ptrdiff_t;

public:
    constexpr FORCEINLINE void Erease(Iterator start, Iterator end) noexcept;

    constexpr FORCEINLINE void Erease(usize start, usize end) noexcept;

    constexpr FORCEINLINE void Erease(Iterator itr) noexcept;

    constexpr FORCEINLINE void Erease(usize index) noexcept;

    constexpr FORCEINLINE void Clear() noexcept
    {
        Utils::Destroy(m_Data, m_Length);
        m_Length = 0;
    }

    constexpr FORCEINLINE bool PopBack() noexcept
    {
        if (m_Length <= 0)
            return false;
        m_Data[--m_Length].~T();
        return m_Length;
    }

    constexpr FORCEINLINE bool PopFront() noexcept
    {
        if (m_Length <= 0)
            return false;
        T* data = m_Data;
        data[0].~T();
        // This is safe slot 1 is moved to slot 0 and so on slot n+1 will be moved in slot n...
        Utils::Move(data, data + 1, --m_Length);
        return m_Length;
    }

    constexpr FORCEINLINE usize Size() const noexcept
    {
        return m_Length;
    }

    constexpr FORCEINLINE usize Length() const noexcept
    {
        return this->Size();
    }

    constexpr FORCEINLINE bool IsEmpty() const noexcept
    {
        return this->Size() == 0;
    }

    constexpr FORCEINLINE T* Back() const noexcept
    {
        ASSERTF(!this->IsEmpty(), "Can't access the last element while the container is empty !");
        return &m_Data[m_Length - 1];
    }

    constexpr FORCEINLINE T* Front() const noexcept
    {
        ASSERTF(!this->IsEmpty(), "Can't access the first element while the container is empty !");
        return &m_Data[0];
    }

    constexpr FORCEINLINE T* Data() const noexcept
    {
        return this->Front();
    }

    constexpr FORCEINLINE T Get(usize i) const noexcept
    {
        ASSERTF(i < m_Size, "Can't access the %llu th element, out of range [0..%llu) !", i, m_Size);
        return m_Data[i];
    }

    constexpr FORCEINLINE T& At(usize i) noexcept
    {
        ASSERTF(i < m_Size, "Can't access the %llu th element, out of range [0..%llu) !", i, m_Size);
        return m_Data[i];
    }

    constexpr FORCEINLINE const T& At(usize i) const noexcept
    {
        ASSERTF(i < m_Size, "Can't access the %llu th element, out of range [0..%llu) !", i, m_Size);
        return m_Data[i];
    }

    constexpr FORCEINLINE T& operator[](usize i) noexcept
    {
        return this->At(i);
    }

    constexpr FORCEINLINE const T& operator[](usize i) const noexcept
    {
        return this->At(i);
    }

    constexpr FORCEINLINE iterator begin() const noexcept
    {
        return Iterator(m_Data);
    }

    constexpr FORCEINLINE iterator end() const noexcept
    {
        return Iterator(m_Data + m_Length);
    }

    constexpr FORCEINLINE const_iterator cbegin() const noexcept
    {
        return CIterator(m_Data);
    }

    constexpr FORCEINLINE const_iterator cend() const noexcept
    {
        return CIterator(m_Data + m_Length);
    }

    constexpr FORCEINLINE const_reverse_iterator rbegin() const noexcept
    {
        return const_reverse_iterator(this->end());
    }

    constexpr FORCEINLINE const_reverse_iterator rend() const noexcept
    {
        return const_reverse_iterator(this->begin());
    }

    constexpr FORCEINLINE const_reverse_iterator crbegin() const noexcept
    {
        return const_reverse_iterator(this->end());
    }

    constexpr FORCEINLINE const_reverse_iterator  crend() const noexcept
    {
        return const_reverse_iterator(this->begin());
    }

    constexpr FORCEINLINE friend void Swap(VectorBase<T>& first, VectorBase<T>& second) noexcept
    {
        std::swap(first.m_Data, second.m_Data);
        std::swap(first.m_Length, second.m_Length);
    }

    constexpr FORCEINLINE friend void swap(VectorBase<T>& first, VectorBase<T>& second) noexcept
    {
        Swap(first, second);
    }

protected:
    constexpr FORCEINLINE VectorBase() noexcept
        : m_Data(nullptr), m_Length(0)
    {

    }

    constexpr FORCEINLINE VectorBase(T* data, usize sz = 0) noexcept
        : m_Data(data), m_Length(sz)
    {

    }

    constexpr VectorBase(VectorBase&& other) = default;

    constexpr VectorBase& operator=(VectorBase&& other) = default;

    constexpr ~VectorBase() = default;

    template<typename... Args>
    constexpr FORCEINLINE T& Emplace(usize i, Args&&... args) noexcept;

    template<typename... Args>
    constexpr FORCEINLINE T& EmplaceBack(Args&&... args) noexcept;

    constexpr FORCEINLINE T& PushBack(const T& obj) noexcept
    {
        return this->EmplaceBack(obj);
    }

    template<typename... Args>
    constexpr FORCEINLINE T& EmplaceFront(Args&&... args) noexcept
    {
        return this->Emplace(0, std::forward<Args>(args)...);
    }

    constexpr FORCEINLINE T& PushFront(const T& obj) noexcept
    {
        return this->Insert(0, obj);
    }

    constexpr FORCEINLINE T& Insert(usize i, const T& obj) noexcept
    {
        return this->Emplace(i, obj);
    }

    constexpr FORCEINLINE void CopyFrom(const T* data, usize length)
    {
        Utils::CopyConstruct(m_Data, data, length);
        m_Length = length;
    }

    constexpr FORCEINLINE void MoveFrom(T* data, usize length) noexcept
    {
        Utils::MoveConstruct(m_Data, data, length);
        m_Length = length;
    }

    constexpr FORCEINLINE void Destroy() noexcept
    {
        Utils::Destroy(m_Data, m_Length);
    }

protected:
    T*    m_Data;
    usize m_Length;
};

template<typename T>
template<typename... Args>
constexpr FORCEINLINE T& VectorBase<T>::EmplaceBack(Args&&... args) noexcept
{
    T* data = m_Data;
    auto newLen = m_Length++;
    new (data + newLen) T(std::forward<Args>(args)...);
    return data[newLen];
}

template<typename T>
template<typename... Args>
constexpr FORCEINLINE T& VectorBase<T>::Emplace(usize i, Args&&... args) noexcept
{
    T* data = m_Data;
    T* dest = data + i;
    // shift all of this to keep place for the new element
    Utils::MoveConstructBackward(data + 1, data, m_Length - 1, i);
    new (dest) T(std::forward<Args>(args)...);
    m_Length++;
    return *dest;
}

template<typename T>
constexpr FORCEINLINE void VectorBase<T>::Erease(Iterator start, Iterator end) noexcept
{
    auto len = m_Length;
    T* data = m_Data;
    TRE_ASSERTF(end >= start, "end must be greater than start");
    TRE_ASSERTF(start < data + len && end <= data + len, "[%" SZu "..%" SZu "] interval isn't included in the range [0..%" SZu "]", start - data, end - data, len);
    const usize size = usize(end - start);
    if (size == 0)
        return;
    usize endIdx = usize(data + len - end);
    Utils::Destroy(start, size);
    Utils::MoveConstruct(start, end, len - endIdx);
    m_Length -= size;
}

template<typename T>
constexpr FORCEINLINE void VectorBase<T>::Erease(usize start, usize end) noexcept
{
    auto begin = this->begin();
    return this->Erease(begin + start, begin + end);
}

template<typename T>
constexpr FORCEINLINE void VectorBase<T>::Erease(Iterator itr) noexcept
{
    auto len = m_Length;
    T* data = m_Data;
    T* itrPtr = itr;
    TRE_ASSERTF((itrPtr >= data && itrPtr < data + len), "The given iterator doesn't belong to the Vector.");

    usize end = usize(data + len - itrPtr);
    Utils::Destroy(itrPtr, 1);
    Utils::MoveConstruct(itrPtr, itrPtr + 1, end - 1);
    m_Length -= 1;
}

template<typename T>
constexpr FORCEINLINE void VectorBase<T>::Erease(usize index) noexcept
{
    this->Erease(this->begin() + index);
}

TRE_NS_END


#endif // VECTORBASE_HPP
