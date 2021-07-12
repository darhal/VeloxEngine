#pragma once

#include <initializer_list>
#include <iterator>
#include <utility>

#include <Core/Misc/Defines/Common.hpp>
#include <Core/Misc/Defines/Debug.hpp>
#include <Core/Memory/Memory.hpp>
#include <Core/Memory/GenericAllocator.hpp>
#include <Core/DataStructure/RandomAccessIterator.hpp>

TRE_NS_START

template<typename T, AllocConcept Alloc = GenericAllocator>
class Vector : public Alloc
{
public:
    using Iterator  = RandomAccessIterator<T>;
    using CIterator = RandomAccessIterator<const T>;
    using value_type = T;

public:
    CONSTEXPR const static usize DEFAULT_CAPACITY       = 16;
    CONSTEXPR const static usize DEFAULT_GROW_SIZE      = 2;
    // TODO : take into account alignement in the statment below :
    CONSTEXPR const static usize STATIC_ELEMENTS_COUNT  = Alloc::Traits::STATIC_CAP / sizeof(T);

public:
    constexpr FORCEINLINE Vector() noexcept;

    constexpr FORCEINLINE Vector(usize sz);

    constexpr FORCEINLINE Vector(usize sz, const T& obj);

    template<usize S>
    constexpr FORCEINLINE Vector(const T(&arr)[S]);

    constexpr FORCEINLINE Vector(const T* data, usize size);

    constexpr FORCEINLINE ~Vector();

    constexpr FORCEINLINE bool Reserve(usize sz);

    template<typename... Args>
    constexpr T& EmplaceBack(Args&&... args);

    constexpr FORCEINLINE T& PushBack(const T& obj);

    template<typename... Args>
    constexpr FORCEINLINE T& EmplaceFront(Args&&... args);

    constexpr FORCEINLINE T& PushFront(const T& obj);

    template<typename... Args>
    constexpr FORCEINLINE T& FastEmplaceFront(Args&&... args);

    constexpr FORCEINLINE T& FastPushFront(const T& obj);

    constexpr FORCEINLINE T& Insert(usize i, const T& obj);

    template<typename... Args>
    constexpr T& Emplace(usize i, Args&&... args);

    constexpr FORCEINLINE T& FastInsert(usize i, const T& obj);

    template<typename... Args>
    constexpr T& FastEmplace(usize i, Args&&... args);

    constexpr void Erease(usize start, usize end) noexcept;

    constexpr void Erease(Iterator itr) noexcept;

    constexpr FORCEINLINE void Erease(usize index) noexcept;

    constexpr FORCEINLINE void FastErease(Iterator itr) noexcept;

    constexpr FORCEINLINE void FastErease(usize index) noexcept;

    constexpr FORCEINLINE void Clear() noexcept;

    constexpr FORCEINLINE bool PopBack() noexcept;

    constexpr bool PopFront() noexcept;

    constexpr bool FastPopFront() noexcept;

    constexpr void Fill(usize length, const T& obj = {});

    constexpr FORCEINLINE void Resize(usize newSize);

    constexpr FORCEINLINE void Append(const Vector<T, Alloc>& other);

    constexpr FORCEINLINE void Append(Vector<T, Alloc>&& other);

    constexpr FORCEINLINE bool IsEmpty() const noexcept;

    constexpr FORCEINLINE usize Capacity() const noexcept;

    constexpr FORCEINLINE usize Length() const noexcept;

    constexpr FORCEINLINE usize Size() const noexcept;

    constexpr FORCEINLINE T* Back() const noexcept;

    constexpr FORCEINLINE T* Front() const noexcept;

    constexpr FORCEINLINE T* Data() const noexcept { return this->Front(); };

    constexpr FORCEINLINE T& Get(usize i) noexcept;

    constexpr FORCEINLINE T& At(usize i) noexcept;

    constexpr FORCEINLINE T& operator[](usize i) noexcept;

    constexpr FORCEINLINE const T& At(usize i) const noexcept;

    constexpr FORCEINLINE const T& operator[](usize i) const noexcept;

    constexpr FORCEINLINE Vector(const Vector<T, Alloc>& other);

    constexpr FORCEINLINE Vector& operator=(const Vector<T, Alloc>& other);

    constexpr FORCEINLINE Vector(Vector<T, Alloc>&& other) noexcept;

    constexpr FORCEINLINE Vector& operator=(Vector<T, Alloc>&& other) noexcept;

    constexpr FORCEINLINE Vector& operator+=(const Vector<T, Alloc>& other);

    constexpr FORCEINLINE Vector& operator+=(Vector<T, Alloc>&& other);

    constexpr FORCEINLINE T* StealPtr() noexcept;

    constexpr FORCEINLINE const Iterator begin() const noexcept
    {
        return Iterator(m_Data);
    }

    constexpr FORCEINLINE const Iterator end() const noexcept
    {
        return Iterator(m_Data + m_Length);
    }

    constexpr FORCEINLINE Iterator begin() noexcept
    {
        return Iterator(m_Data);
    }

    constexpr FORCEINLINE Iterator end() noexcept
    {
        return Iterator(m_Data + m_Length);
    }

    constexpr FORCEINLINE CIterator cbegin() const noexcept
    {
        return CIterator(m_Data);
    }

    constexpr FORCEINLINE CIterator cend() const noexcept
    {
        return CIterator(m_Data + m_Length);
    }

    constexpr FORCEINLINE friend void Swap(Vector<T, Alloc>& first, Vector<T, Alloc>& second) noexcept
    {
        std::swap(first.m_Data, second.m_Data);
        std::swap(first.m_Length, second.m_Length);
        std::swap(first.m_Capacity, second.m_Capacity);
    }

    constexpr FORCEINLINE friend void swap(Vector<T, Alloc>& first, Vector<T, Alloc>& second) noexcept
    {
        Swap(first, second);
    }

private:
    constexpr FORCEINLINE void ReserveHelper(usize nCap);

    constexpr FORCEINLINE void Free(T* data, usize sz);

    constexpr FORCEINLINE static usize GetAllocCapIfSizeIsLessThan(usize sz)
    {
        return sz > STATIC_ELEMENTS_COUNT ? sz : STATIC_ELEMENTS_COUNT;
    }

    constexpr FORCEINLINE T* GetInitialData()
    {
        if constexpr (STATIC_ELEMENTS_COUNT != 0) {
            return this->template Allocate<T>(STATIC_ELEMENTS_COUNT);
        }else{
            return nullptr;
        }
    }

    constexpr FORCEINLINE void HandleMoveAfterRealloc(T* newData, usize size)
    {
        auto HandleRealloc = [this](T* newData, usize size) {
            Utils::MoveConstruct(newData, m_Data, size);
            this->FreeMemory(m_Data);
        };

        if constexpr (Alloc::Traits::HAVE_REALLOC) {
            if (newData != m_Data) {
                HandleRealloc(newData, size);
                m_Data = newData;
            }
        }else{
            HandleRealloc(newData, size);
        }
    }

private:
    T*    m_Data;
    usize m_Length;
    usize m_Capacity;
};

template<typename T, AllocConcept Alloc>
constexpr FORCEINLINE Vector<T, Alloc>::Vector() noexcept
    : m_Data(GetInitialData()), m_Length(0), m_Capacity(STATIC_ELEMENTS_COUNT)
{

}

template<typename T, AllocConcept Alloc>
constexpr FORCEINLINE Vector<T, Alloc>::Vector(usize sz)
    : m_Data(this->template Allocate<T>(sz)), m_Length(0),
      m_Capacity(GetAllocCapIfSizeIsLessThan(sz))
{

}

template<typename T, AllocConcept Alloc>
constexpr FORCEINLINE Vector<T, Alloc>::Vector(usize sz, const T& obj)
    : m_Data(this->template Allocate<T>(sz)), m_Length(0),
      m_Capacity(GetAllocCapIfSizeIsLessThan(sz))
{
    this->Fill(m_Capacity, obj);
}

template<typename T, AllocConcept Alloc>
constexpr FORCEINLINE Vector<T, Alloc>::Vector(const T* data, usize size)
    : m_Data(this->template Allocate<T>(size)), m_Length(size),
      m_Capacity(GetAllocCapIfSizeIsLessThan(size))
{
    Utils::CopyConstruct(m_Data, data, m_Length);
}

template<typename T, AllocConcept Alloc>
template<usize S>
constexpr FORCEINLINE Vector<T, Alloc>::Vector(const T(&arr)[S])
    : Vector(arr, S)
{

}

/*template<typename T, AllocConcept Alloc>
constexpr FORCEINLINE Vector<T, Alloc>::Vector(const std::initializer_list<T>& list)
    : Vector(list.begin(), list.size())
{

}*/

template<typename T, AllocConcept Alloc>
constexpr FORCEINLINE Vector<T, Alloc>::~Vector()
{
    if (m_Data != NULL) {
        this->Free(m_Data, m_Length);
        m_Data = NULL;
    }
}

template<typename T, AllocConcept Alloc>
constexpr void Vector<T, Alloc>::Fill(usize length, const T& obj)
{
    this->Reserve(length);
    Utils::MemSet(m_Data, obj, length);
    m_Length = length;
}

template<typename T, AllocConcept Alloc>
template<typename... Args>
constexpr T& Vector<T, Alloc>::EmplaceBack(Args&&... args)
{
    this->Reserve(m_Length + 1);
    new (m_Data + m_Length) T(std::forward<Args>(args)...);
    return *(m_Data + (m_Length++));
}

template<typename T, AllocConcept Alloc>
constexpr FORCEINLINE T& Vector<T, Alloc>::PushBack(const T& obj)
{
    return this->EmplaceBack(obj);
}

template<typename T, AllocConcept Alloc>
constexpr FORCEINLINE bool Vector<T, Alloc>::PopBack() noexcept
{
    if (m_Length <= 0)
        return false;
    m_Data[--m_Length].~T();
    return m_Length;
}

template<typename T, AllocConcept Alloc>
constexpr bool Vector<T, Alloc>::PopFront() noexcept
{
    if (m_Length <= 0) 
        return false;

    m_Data[0].~T();
    // This is safe slot 1 is moved to slot 0 and so on slot n+1 will be moved in slot n...
    Utils::Move(m_Data, m_Data + 1, --m_Length);
    return m_Length;
}

template<typename T, AllocConcept Alloc>
constexpr bool Vector<T, Alloc>::FastPopFront() noexcept
{
    if (m_Length <= 0)
        return false;

    m_Data[0].~T();
    if (--m_Length != 0)
        new (m_Data) T(std::move(m_Data[m_Length + 1]));
    return false;
}

template<typename T, AllocConcept Alloc>
constexpr FORCEINLINE bool Vector<T, Alloc>::Reserve(usize sz)
{
    if (sz < m_Capacity)
        return false;

    // sz = sz ? sz * DEFAULT_GROW_SIZE : DEFAULT_CAPACITY;
    sz = (sz << DEFAULT_GROW_SIZE) + DEFAULT_CAPACITY;
    this->ReserveHelper(sz);
    return true;
}

template<typename T, AllocConcept Alloc>
constexpr FORCEINLINE void Vector<T, Alloc>::ReserveHelper(usize nCap)
{
    // I think this can be optimized! to just copy and dont delete the thing or use move ctor.
    T* newData = this->template Reallocate<T>(m_Data, nCap);
    this->HandleMoveAfterRealloc(newData, m_Length);
    m_Capacity = nCap;
}

template<typename T, AllocConcept Alloc>
constexpr FORCEINLINE void Vector<T, Alloc>::Free(T* data, usize sz)
{
    Utils::Destroy(data, sz);
    this->FreeMemory(data);
}

template<typename T, AllocConcept Alloc>
constexpr void Vector<T, Alloc>::Resize(usize newSize)
{
    if (newSize < m_Length) {
        usize offset = m_Length - newSize;
        Utils::Destroy(m_Data + newSize, offset);
        m_Length = newSize;
    } else if (newSize > m_Length) {
        this->Reserve(newSize);
        m_Length = newSize;
    }
}

template<typename T, AllocConcept Alloc>
constexpr FORCEINLINE T& Vector<T, Alloc>::Insert(usize i, const T& obj)
{
    return this->Emplace(i, obj);
}

template<typename T, AllocConcept Alloc>
constexpr FORCEINLINE T& Vector<T, Alloc>::PushFront(const T& obj)
{
    return this->Insert(0, obj);
}

template<typename T, AllocConcept Alloc>
template<typename... Args>
constexpr FORCEINLINE T& Vector<T, Alloc>::FastEmplaceFront(Args&&... args)
{
    return this->FastEmplace(0, std::forward<Args>(args)...);
}

template<typename T, AllocConcept Alloc>
constexpr FORCEINLINE T& Vector<T, Alloc>::FastPushFront(const T& obj)
{
    return this->FastInsert(0, obj);
}

template<typename T, AllocConcept Alloc>
template<typename... Args>
constexpr T& Vector<T, Alloc>::Emplace(usize i, Args&&... args)
{
    TRE_ASSERTF(i <= m_Length, "Given index is out of bound please choose from [0..%" SZu "].", m_Length);
    auto cap = m_Capacity;
    auto len = m_Length;
    auto data = m_Data;

    if (len + 1 >= cap) {
        // usize nCap = m_Capacity ? m_Capacity * DEFAULT_GROW_SIZE : DEFAULT_CAPACITY;
        usize nCap = (++m_Length << DEFAULT_GROW_SIZE) + DEFAULT_CAPACITY;
        T* newData = this->template Reallocate<T>(data, nCap);
        T* dest = newData + i;
        Utils::MoveConstructForward(newData + 1, data, i, len);
        new (dest) T(std::forward<Args>(args)...);
        this->HandleMoveAfterRealloc(newData, i);
        m_Capacity = nCap;
        return *(dest);
    } else {
        T* dest = data + i;
        // shift all of this to keep place for the new element
        Utils::MoveConstructBackward(m_Data + 1, data, len - 1, i);
        new (dest) T(std::forward<Args>(args)...);
        m_Length++;
        return *(dest);
    }
}

template<typename T, AllocConcept Alloc>
constexpr FORCEINLINE T& Vector<T, Alloc>::FastInsert(usize i, const T& obj)
{
    return this->FastEmplace(i, obj);
}

template<typename T, AllocConcept Alloc>
template<typename... Args>
constexpr T& Vector<T, Alloc>::FastEmplace(usize i, Args&&... args)
{
    TRE_ASSERTF(i <= m_Length, "Given index is out of bound please choose from [0..%" SZu "].", m_Length);

    if (m_Length + 1 >= m_Capacity) // The default way might be faster as we have to reallocate the memory and copy the objects anyways
        return this->Emplace(i, std::forward<Args>(args)...);

    T* element = m_Data + i;
    
    if (i != m_Length) [[likely]] {
        T* last = m_Data + m_Length;
        T temp(std::move(*element));
        new (element) T(std::forward<Args>(args)...);
        new (last) T(std::move(temp));
    } else [[unlikely]] {
        new (element) T(std::forward<Args>(args)...);
    }

    m_Length++;
    return *element;
}

template<typename T, AllocConcept Alloc>
template<typename ...Args>
constexpr FORCEINLINE T& Vector<T, Alloc>::EmplaceFront(Args&&... args)
{
    return this->Emplace(0, std::forward<Args>(args)...);
}

template<typename T, AllocConcept Alloc>
constexpr FORCEINLINE void Vector<T, Alloc>::Append(const Vector<T, Alloc>& other)
{
    this->Reserve(m_Length + other.m_Length);
    Utils::Copy(m_Data + m_Length, other.m_Data, other.m_Length);
    m_Length += other.m_Length;
}

template<typename T, AllocConcept Alloc>
constexpr FORCEINLINE void Vector<T, Alloc>::Append(Vector<T, Alloc>&& other)
{
    this->Reserve(m_Length + other.m_Length);
    Utils::Move(m_Data + m_Length, other.m_Data, other.m_Length);
    m_Length += other.m_Length;
    other.m_Length = 0;
}

template<typename T, AllocConcept Alloc>
constexpr FORCEINLINE Vector<T, Alloc>& Vector<T, Alloc>::operator+=(const Vector<T, Alloc>& other)
{
    this->Append(other);
    return *this;
}

template<typename T, AllocConcept Alloc>
constexpr FORCEINLINE Vector<T, Alloc>& Vector<T, Alloc>::operator+=(Vector<T, Alloc>&& other)
{
    this->Append(std::forward<Vector<T, Alloc>>(other));
    return *this;
}

template<typename T, AllocConcept Alloc>
constexpr void Vector<T, Alloc>::Erease(usize start, usize end) noexcept
{
    TRE_ASSERTF(start < m_Length && end <= m_Length, "[%" SZu "..%" SZu "] interval isn't included in the range [0..%" SZu "]", start, end, m_Length);
    TRE_ASSERTF(end >= start, "end must be greater than start");
    const usize size = end - start;
    if (size == 0) 
        return;

    Utils::Destroy(m_Data + start, size);
    Utils::MoveConstruct(m_Data + start, m_Data + end, m_Length - end);
    m_Length -= size;
}

template<typename T, AllocConcept Alloc>
constexpr void Vector<T, Alloc>::Erease(Iterator itr) noexcept
{
    T* itr_ptr = itr.GetPtr();
    TRE_ASSERTF((itr_ptr < m_Data + m_Length && itr_ptr >= m_Data), "The given iterator doesn't belong to the Vector.");
    Utils::Destroy(itr_ptr, 1);
    usize start = usize(itr_ptr - m_Data);
    usize end = usize(m_Data + m_Length - itr_ptr);
    Utils::MoveConstruct(m_Data + start, m_Data + start + 1, end - 1);
    m_Length -= 1;
}

template<typename T, AllocConcept Alloc>
constexpr FORCEINLINE void Vector<T, Alloc>::Erease(usize index) noexcept
{
    return this->Erease(this->begin() + index);
}

template<typename T, AllocConcept Alloc>
constexpr FORCEINLINE void Vector<T, Alloc>::FastErease(Iterator itr) noexcept
{
    T* itr_ptr = itr.GetPtr();
    TRE_ASSERTF((itr_ptr < m_Data + m_Length && itr_ptr >= m_Data), "The given iterator doesn't belong to the Vector.");
    (*itr_ptr).~T();
    T* last_ptr = m_Data + m_Length - 1;
    new (itr_ptr) T(std::move(*last_ptr));
    m_Length -= 1;
}

template<typename T, AllocConcept Alloc>
constexpr FORCEINLINE void Vector<T, Alloc>::FastErease(usize index) noexcept
{
    return this->FastErease(this->begin() + index);
}

template<typename T, AllocConcept Alloc>
constexpr FORCEINLINE T* Vector<T, Alloc>::StealPtr() noexcept
{
    T* data_ptr = m_Data;
    m_Length = 0;
    m_Capacity = 0;
    m_Data = NULL;
    return data_ptr;
}

template<typename T, AllocConcept Alloc>
constexpr FORCEINLINE void Vector<T, Alloc>::Clear() noexcept
{
    Utils::Destroy(m_Data, m_Length);
    m_Length = 0;
}

template<typename T, AllocConcept Alloc>
constexpr FORCEINLINE bool Vector<T, Alloc>::IsEmpty() const noexcept
{
    return this->Size() == 0;
}

template<typename T, AllocConcept Alloc>
constexpr FORCEINLINE usize Vector<T, Alloc>::Capacity() const noexcept
{
    return m_Capacity;
}

template<typename T, AllocConcept Alloc>
constexpr FORCEINLINE usize Vector<T, Alloc>::Length() const noexcept
{
    return m_Length;
}

template<typename T, AllocConcept Alloc>
constexpr FORCEINLINE usize Vector<T, Alloc>::Size() const noexcept
{
    return m_Length;
}

template<typename T, AllocConcept Alloc>
constexpr FORCEINLINE T* Vector<T, Alloc>::Back() const noexcept
{
    if (m_Length == 0)
        return NULL;

    return m_Data + m_Length - 1;
}

template<typename T, AllocConcept Alloc>
constexpr FORCEINLINE T* Vector<T, Alloc>::Front() const noexcept
{
    return m_Data;
}

/*template<typename T, AllocConcept Alloc>
const T* Vector<T, Alloc>::At(usize i)
{
    ASSERTF((i >= m_Length), "Bad usage of vector function At index out of bounds");
    return &m_Data[i];
}

template<typename T, AllocConcept Alloc>
const T* Vector<T, Alloc>::operator[](usize i)
{
    if (i >= m_Length) return NULL;
    return At(i);
}*/

template<typename T, AllocConcept Alloc>
constexpr FORCEINLINE T& Vector<T, Alloc>::Get(usize i) noexcept
{
    return this->At(i);
}

template<typename T, AllocConcept Alloc>
constexpr FORCEINLINE T& Vector<T, Alloc>::At(usize i) noexcept
{
    TRE_ASSERTF(i < m_Length, "Bad usage of vector function At index out of bounds");
    return m_Data[i];
}

template<typename T, AllocConcept Alloc>
constexpr FORCEINLINE T& Vector<T, Alloc>::operator[](usize i) noexcept
{
    return this->At(i);
}

template<typename T, AllocConcept Alloc>
constexpr FORCEINLINE const T& Vector<T, Alloc>::At(usize i) const noexcept
{
    TRE_ASSERTF((i < m_Length), "Bad usage of vector function At index out of bounds");
    return m_Data[i];
}

template<typename T, AllocConcept Alloc>
constexpr FORCEINLINE const T& Vector<T, Alloc>::operator[](usize i) const noexcept
{
    return this->At(i);
}

template<typename T, AllocConcept Alloc>
constexpr FORCEINLINE Vector<T, Alloc>::Vector(const Vector<T, Alloc>& other) :
    Alloc(other), m_Data(nullptr), m_Length(other.m_Length), m_Capacity(other.m_Capacity)
{
    if (m_Capacity) {
        m_Data = this->template Allocate<T>(m_Capacity);
        Utils::Copy(other.m_Data, m_Data, m_Length);
    }
}

template<typename T, AllocConcept Alloc>
constexpr FORCEINLINE Vector<T, Alloc>& Vector<T, Alloc>::operator=(const Vector<T, Alloc>& other)
{
    Vector<T, Alloc> tmp(other);
    Swap(*this, tmp);
    return *this;
}

template<typename T, AllocConcept Alloc>
constexpr FORCEINLINE Vector<T, Alloc>::Vector(Vector<T, Alloc>&& other) noexcept
    : Alloc(other), m_Data(other.m_Data), m_Length(other.m_Length), m_Capacity(other.m_Capacity)
{
    other.m_Data = NULL;
}

template<typename T, AllocConcept Alloc>
constexpr FORCEINLINE Vector<T, Alloc>& Vector<T, Alloc>::operator=(Vector<T, Alloc>&& other) noexcept
{
    Vector<T, Alloc> tmp(std::move(other));
    Swap(*this, tmp);
    return *this;
}

template<typename T, AllocConcept Alloc>
constexpr void swap(typename Vector<T, Alloc>::Iterator& a, typename Vector<T, Alloc>::Iterator& b) noexcept
{
    std::swap(*a, *b);
}

TRE_NS_END
