#pragma once

#include <initializer_list>
#include <iterator>
#include <utility>

#include <Core/Misc/Defines/Common.hpp>
#include <Core/Misc/Defines/Debug.hpp>
#include <Core/Utils/Memory.hpp>

TRE_NS_START

template<typename T>
class Vector
{
public:
    template<typename PointerType>
    class GIterator;

    typedef GIterator<T> Iterator;
    typedef GIterator<const T> CIterator;

public:
    CONSTEXPR const static usize DEFAULT_CAPACITY = 8;
    CONSTEXPR const static usize DEFAULT_GROW_SIZE = 3;

public:
    FORCEINLINE Vector() noexcept;

    FORCEINLINE Vector(usize sz) noexcept;

    FORCEINLINE Vector(usize sz, const T& obj);
    
    Vector(const std::initializer_list<T>& list);

    template<usize S>
    FORCEINLINE Vector(T(&arr)[S]);

    FORCEINLINE Vector(T* data, usize size);

    FORCEINLINE ~Vector();

    FORCEINLINE bool Reserve(usize sz);

    template<typename... Args>
    T& EmplaceBack(Args&&... args);

    FORCEINLINE T& PushBack(const T& obj);

    template<typename... Args>
    FORCEINLINE T& EmplaceFront(Args&&... args);

    FORCEINLINE T& PushFront(const T& obj);

    template<typename... Args>
    FORCEINLINE T& EmplaceFrontFast(Args&&... args);

    FORCEINLINE T& PushFrontFast(const T& obj);

    FORCEINLINE T& Insert(usize i, const T& obj);

    template<typename... Args>
    T& Emplace(usize i, Args&&... args);

    FORCEINLINE T& InsertFast(usize i, const T& obj);

    template<typename... Args>
    T& EmplaceFast(usize i, Args&&... args);

    void Erease(usize start, usize end) noexcept;

    void Erease(Iterator itr) noexcept;

    FORCEINLINE void Erease(usize index) noexcept;

    FORCEINLINE void EreaseFast(Iterator itr) noexcept;

    FORCEINLINE void EreaseFast(usize index) noexcept;

    FORCEINLINE void Clear() noexcept;

    FORCEINLINE bool PopBack() noexcept;

    bool PopFront() noexcept;

    bool PopFrontFast() noexcept;

    void Fill(usize length, const T& obj = {});

    FORCEINLINE void Resize(usize newSize);

    FORCEINLINE void Append(const Vector<T>& other);

    FORCEINLINE void Append(Vector<T>&& other);

    FORCEINLINE bool IsEmpty() const noexcept;

    FORCEINLINE usize Capacity() const noexcept;

    FORCEINLINE usize Length() const noexcept;

    FORCEINLINE usize Size() const noexcept;

    FORCEINLINE T* Back() const noexcept;

    FORCEINLINE T* Front() const noexcept;

    FORCEINLINE T* Data() const noexcept { return this->Front(); };

    FORCEINLINE T& Get(usize i) noexcept;

    FORCEINLINE T& At(usize i) noexcept;

    FORCEINLINE T& operator[](usize i) noexcept;

    FORCEINLINE const T& At(usize i) const noexcept;

    FORCEINLINE const T& operator[](usize i) const noexcept;

    FORCEINLINE Iterator begin() noexcept;

    FORCEINLINE Iterator end() noexcept;

    FORCEINLINE const Iterator begin() const noexcept;

    FORCEINLINE const Iterator end() const noexcept;

    FORCEINLINE CIterator cbegin() const noexcept;

    FORCEINLINE CIterator cend() const noexcept;

    FORCEINLINE Vector(const Vector<T>& other);

    FORCEINLINE Vector& operator=(const Vector<T>& other);

    FORCEINLINE Vector(Vector<T>&& other) noexcept;

    FORCEINLINE Vector& operator=(Vector<T>&& other) noexcept;

    FORCEINLINE Vector& operator+=(const Vector<T>& other);

    FORCEINLINE Vector& operator+=(Vector<T>&& other);

    FORCEINLINE T* StealPtr() noexcept;

    FORCEINLINE friend void Swap(Vector<T>& first, Vector<T>& second) noexcept
    {
        std::swap(first.m_Data, second.m_Data);
        std::swap(first.m_Length, second.m_Length);
        std::swap(first.m_Capacity, second.m_Capacity);
    }

    FORCEINLINE friend void swap(Vector<T>& first, Vector<T>& second) noexcept
    {
        Swap(first, second);
    }
private:
    FORCEINLINE void Reallocate(usize nCap);

private:
    T*    m_Data;
    usize m_Length;
    usize m_Capacity;

public:
    template<typename DataType>
    class GIterator : public std::iterator<std::random_access_iterator_tag, DataType, ptrdiff_t, DataType*, DataType&>
    {
    public:
        GIterator() noexcept : m_Current(m_Data) { }
        GIterator(DataType* node) noexcept : m_Current(node) { }

        GIterator(const GIterator<DataType>& rawIterator) = default;
        GIterator& operator=(const GIterator<DataType>& rawIterator) = default;

        GIterator(GIterator<DataType>&& rawIterator) = default;
        GIterator& operator=(GIterator<DataType>&& rawIterator) = default;
        
        ~GIterator() {}

        bool operator!=(const GIterator& iterator) { return m_Current != iterator.m_Current; }

        DataType& operator*() { return *m_Current; }
        const DataType& operator*() const { return (*m_Current); }

        DataType* operator->() { return m_Current; }
        const DataType* operator->() const { return m_Current; }

        DataType* GetPtr() const { return m_Current; }
        const DataType* GetConstPtr() const { return m_Current; }

        GIterator<DataType>& operator+=(const ptrdiff_t& movement) { m_Current += movement; return (*this); }
        GIterator<DataType>& operator-=(const ptrdiff_t& movement) { m_Current -= movement; return (*this); }
        GIterator<DataType>& operator++() { m_Current++; return (*this); }
        GIterator<DataType>& operator--() { m_Current--; return (*this); }
        GIterator<DataType>  operator++(int) { auto temp(*this); ++m_Current; return temp; }
        GIterator<DataType>  operator--(int) { auto temp(*this); --m_Current; return temp; }
        GIterator<DataType>  operator+(const ptrdiff_t& movement) { 
            auto oldPtr = m_Current; 
            m_Current += movement; 
            auto temp(*this); m_Current = oldPtr; 
            return temp; 
        }

        GIterator<DataType>  operator-(const ptrdiff_t& movement) { 
            auto oldPtr = m_Current; 
            m_Current -= movement; 
            auto temp(*this); 
            m_Current = oldPtr; 
            return temp; 
        }
    private:
        DataType* m_Current;
    };
};

template<typename T>
FORCEINLINE Vector<T>::Vector() noexcept
    : m_Data(NULL), m_Length(0), m_Capacity(0)
{
}

template<typename T>
FORCEINLINE Vector<T>::Vector(usize sz) noexcept
    : m_Data(Utils::Allocate<T>(sz)), m_Length(0), m_Capacity(sz)
{
}

template<typename T>
FORCEINLINE Vector<T>::Vector(usize sz, const T& obj)
    : m_Data(Utils::Allocate<T>(sz)), m_Length(0), m_Capacity(sz)
{
    this->Fill(m_Capacity, obj);
}

template<typename T>
template<usize S>
FORCEINLINE Vector<T>::Vector(T(&arr)[S])
    : m_Data(Utils::Allocate<T>(S)), m_Length(S), m_Capacity(S)
{
    Utils::CopyConstruct(m_Data, arr, m_Length);
}

template<typename T>
FORCEINLINE Vector<T>::Vector(T* data, usize size)
    : m_Data(Utils::Allocate<T>(size)), m_Length(size), m_Capacity(size)
{
    Utils::CopyConstruct(m_Data, data, m_Length);
}

template<typename T>
Vector<T>::Vector(const std::initializer_list<T>& list) 
    : m_Data(Utils::Allocate<T>(list.size())), m_Length(list.size()), m_Capacity(list.size())
{
    T* ptr = m_Data;

    for (const T& obj : list) {
        new (ptr) T(obj);
        ptr++;
    }
}

template<typename T>
FORCEINLINE Vector<T>::~Vector()
{
    if (m_Data != NULL) {
        Utils::Free(m_Data, m_Length);
        m_Data = NULL;
    }
}

template<typename T>
void Vector<T>::Fill(usize length, const T& obj)
{
    this->Reserve(length);
    Utils::MemSet(m_Data, obj, length);
    m_Length = length;
}

template<typename T>
template<typename... Args>
T& Vector<T>::EmplaceBack(Args&&... args)
{
    this->Reserve(m_Length + 1);
    new (m_Data + m_Length) T(std::forward<Args>(args)...);
    return *(m_Data + (m_Length++));
}

template<typename T>
FORCEINLINE T& Vector<T>::PushBack(const T& obj)
{
    return this->EmplaceBack(obj);
}

template<typename T>
FORCEINLINE bool Vector<T>::PopBack() noexcept
{
    if (m_Length <= 0)
        return false;

    m_Data[--m_Length].~T();
    return m_Length;
}

template<typename T>
bool Vector<T>::PopFront() noexcept
{
    if (m_Length <= 0) 
        return false;
    
    m_Data[0].~T();
    // This is safe slot 1 is moved to slot 0 and so on slot n+1 will be moved in slot n...
    Utils::Move(m_Data, m_Data + 1, --m_Length);
    return m_Length;
}

template<typename T>
bool Vector<T>::PopFrontFast() noexcept
{
    if (m_Length <= 0)
        return false;

    m_Data[0].~T();

    if (--m_Length != 0) {
        new (m_Data) T(std::move(m_Data[m_Length + 1]));
    }

    return false;
}

template<typename T>
FORCEINLINE bool Vector<T>::Reserve(usize sz)
{
    if (sz < m_Capacity)
        return false;

    // sz = sz ? sz * DEFAULT_GROW_SIZE : DEFAULT_CAPACITY;
    sz = (sz * DEFAULT_GROW_SIZE) + DEFAULT_CAPACITY;
    this->Reallocate(sz);
    return true;
}

template<typename T>
FORCEINLINE void Vector<T>::Reallocate(usize nCap) // I think this can be optimized! to just copy and dont delete the thing or use move ctor.
{
    T* newData = Utils::Allocate<T>(nCap);
    Utils::MoveConstruct(newData, m_Data, m_Length);
    Utils::FreeMemory(m_Data);
    m_Data = newData;
    m_Capacity = nCap;
}

template<typename T>
void Vector<T>::Resize(usize newSize)
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

template<typename T>
FORCEINLINE T& Vector<T>::Insert(usize i, const T& obj)
{
    return this->Emplace(i, obj);
}

template<typename T>
FORCEINLINE T& Vector<T>::PushFront(const T& obj)
{
    return this->Insert(0, obj);
}

template<typename T>
template<typename... Args>
FORCEINLINE T& Vector<T>::EmplaceFrontFast(Args&&... args)
{
    return this->EmplaceFast(0, std::forward<Args>(args)...);
}

template<typename T>
FORCEINLINE T& Vector<T>::PushFrontFast(const T& obj)
{
    return this->InsertFast(0, obj);
}

template<typename T>
template<typename... Args>
T& Vector<T>::Emplace(usize i, Args&&... args)
{
    ASSERTF(i > m_Length, "Given index is out of bound please choose from [0..%" SZu "].", m_Length);

    if (m_Length + 1 >= m_Capacity) {
        // usize nCap = m_Capacity ? m_Capacity * DEFAULT_GROW_SIZE : DEFAULT_CAPACITY;
        usize nCap = (m_Capacity * DEFAULT_GROW_SIZE) + DEFAULT_CAPACITY;
        T* newData = Utils::Allocate<T>(nCap);
        T* dest = newData + i;
        Utils::MoveConstruct(newData, m_Data, i);
        Utils::MoveConstructForward(newData + 1, m_Data, i, m_Length);
        new (dest) T(std::forward<Args>(args)...);
        Utils::FreeMemory(m_Data);
        m_Data = newData;
        m_Capacity = nCap;
        m_Length++;
        return *(dest);
    } else {
        T* dest = m_Data + i;
        // shift all of this to keep place for the new element
        Utils::MoveConstructBackward(m_Data + 1, m_Data, m_Length - 1, i);
        new (dest) T(std::forward<Args>(args)...);
        m_Length++;
        return *(dest);
    }
}

template<typename T>
FORCEINLINE T& Vector<T>::InsertFast(usize i, const T& obj)
{
    return this->EmplaceFast(i, obj);
}

template<typename T>
template<typename... Args>
T& Vector<T>::EmplaceFast(usize i, Args&&... args)
{
    ASSERTF(i > m_Length, "Given index is out of bound please choose from [0..%" SZu "].", m_Length);

    if (m_Length + 1 >= m_Capacity) { // The default way might be faster as we have to reallocate the memory and copy the objects anyways
        return this->Emplace(i, std::forward<Args>(args)...);
    }

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

template<typename T>
template<typename ...Args>
FORCEINLINE T& Vector<T>::EmplaceFront(Args&&... args)
{
    return this->Emplace(0, std::forward<Args>(args)...);
}

template<typename T>
FORCEINLINE void Vector<T>::Append(const Vector<T>& other)
{
    this->Reserve(m_Length + other.m_Length);
    Utils::Copy(m_Data + m_Length, other.m_Data, other.m_Length);
    m_Length += other.m_Length;
}

template<typename T>
FORCEINLINE void Vector<T>::Append(Vector<T>&& other)
{
    this->Reserve(m_Length + other.m_Length);
    Utils::Move(m_Data + m_Length, other.m_Data, other.m_Length);
    m_Length += other.m_Length;
    other.m_Length = 0;
}

template<typename T>
FORCEINLINE Vector<T>& Vector<T>::operator+=(const Vector<T>& other)
{
    this->Append(other);
    return *this;
}

template<typename T>
FORCEINLINE Vector<T>& Vector<T>::operator+=(Vector<T>&& other)
{
    this->Append(std::forward<Vector<T>>(other));
    return *this;
}

template<typename T>
void Vector<T>::Erease(usize start, usize end) noexcept
{
    ASSERTF((start >= m_Length || end > m_Length), "[%" SZu "..%" SZu "] interval isn't included in the range [0..%" SZu "]", start, end, m_Length);
    ASSERTF((end < start), "end must be greater than start");
    
    usize size = end - start;
    if (size == 0) 
        return;

    for (usize i = start; i < end; i++) {
        m_Data[i].~T();
    }

    Utils::MoveConstruct(m_Data + start, m_Data + end, m_Length - end);
    m_Length -= size;
}

template<typename T>
void Vector<T>::Erease(Iterator itr) noexcept
{
    T* itr_ptr = itr.GetPtr();
    ASSERTF((itr_ptr >= m_Data + m_Length || itr_ptr < m_Data), "The given iterator doesn't belong to the Vector.");
    
    (*itr_ptr).~T();
    usize start = usize(itr_ptr - m_Data);
    usize end = usize(m_Data + m_Length - itr_ptr);
    Utils::MoveConstruct(m_Data + start, m_Data + start + 1, end - 1);
    m_Length -= 1;
}

template<typename T>
FORCEINLINE void Vector<T>::Erease(usize index) noexcept
{
    return this->Erease(this->begin() + index);
}

template<typename T>
FORCEINLINE void Vector<T>::EreaseFast(Iterator itr) noexcept
{
    T* itr_ptr = itr.GetPtr();
    ASSERTF((itr_ptr >= m_Data + m_Length || itr_ptr < m_Data), "The given iterator doesn't belong to the Vector.");
    
    (*itr_ptr).~T();
    T* last_ptr = m_Data + m_Length - 1;
    new (itr_ptr) T(std::move(*last_ptr));
    m_Length -= 1;
}

template<typename T>
FORCEINLINE void Vector<T>::EreaseFast(usize index) noexcept
{
    return this->EreaseFast(this->begin() + index);
}

template<typename T>
FORCEINLINE T* Vector<T>::StealPtr() noexcept
{
    T* data_ptr = m_Data;
    m_Length = 0;
    m_Capacity = 0;
    m_Data = NULL;
    return data_ptr;
}

template<typename T>
FORCEINLINE void Vector<T>::Clear() noexcept
{
    Utils::Destroy(m_Data, m_Length);
    m_Length = 0;
}

template<typename T>
FORCEINLINE bool Vector<T>::IsEmpty() const noexcept
{
    return this->Size() == 0;
}

template<typename T>
FORCEINLINE usize Vector<T>::Capacity() const noexcept
{
    return m_Capacity;
}

template<typename T>
FORCEINLINE usize Vector<T>::Length() const noexcept
{
    return m_Length;
}

template<typename T>
FORCEINLINE usize Vector<T>::Size() const noexcept
{
    return m_Length;
}

template<typename T>
FORCEINLINE T* Vector<T>::Back() const noexcept
{
    if (m_Length == 0)
        return NULL;

    return m_Data + m_Length - 1;
}

template<typename T>
FORCEINLINE T* Vector<T>::Front() const noexcept
{
    return m_Data;
}

/*template<typename T>
const T* Vector<T>::At(usize i)
{
    ASSERTF((i >= m_Length), "Bad usage of vector function At index out of bounds");
    return &m_Data[i];
}

template<typename T>
const T* Vector<T>::operator[](usize i)
{
    if (i >= m_Length) return NULL;
    return At(i);
}*/

template<typename T>
FORCEINLINE T& Vector<T>::Get(usize i) noexcept
{
    return At(i);
}

template<typename T>
FORCEINLINE T& Vector<T>::At(usize i) noexcept
{
    ASSERTF((i >= m_Length), "Bad usage of vector function At index out of bounds");
    return m_Data[i];
}

template<typename T>
FORCEINLINE T& Vector<T>::operator[](usize i) noexcept
{
    return At(i);
}

template<typename T>
FORCEINLINE const T& Vector<T>::At(usize i) const noexcept
{
    ASSERTF((i >= m_Length), "Bad usage of vector function At index out of bounds");
    return m_Data[i];
}

template<typename T>
FORCEINLINE const T& Vector<T>::operator[](usize i) const noexcept
{
    return this->At(i);
}

template<typename T>
FORCEINLINE Vector<T>::Vector(const Vector<T>& other) :
    m_Data(nullptr), m_Length(other.m_Length), m_Capacity(other.m_Capacity)
{
    if (m_Capacity) {
        m_Data = Utils::Allocate<T>(m_Capacity);
        Utils::Copy(other.m_Data, m_Data, m_Length);
    }
}

template<typename T>
FORCEINLINE Vector<T>& Vector<T>::operator=(const Vector<T>& other)
{
    Vector<T> tmp(other);
    Swap(*this, tmp);
    return *this;
}

template<typename T>
FORCEINLINE Vector<T>::Vector(Vector<T>&& other) noexcept
    : m_Data(other.m_Data), m_Length(other.m_Length), m_Capacity(other.m_Capacity)
{
    other.m_Data = NULL;
}

template<typename T>
FORCEINLINE Vector<T>& Vector<T>::operator=(Vector<T>&& other) noexcept
{
    Vector<T> tmp(std::move(other));
    Swap(*this, tmp);
    return *this;
}

template<typename T>
FORCEINLINE const typename Vector<T>::Iterator Vector<T>::begin() const noexcept
{
    return Iterator(m_Data);
}

template<typename T>
FORCEINLINE const typename Vector<T>::Iterator Vector<T>::end() const noexcept
{
    return Iterator(m_Data + m_Length);
}

template<typename T>
FORCEINLINE typename Vector<T>::Iterator Vector<T>::begin() noexcept
{
    return Iterator(m_Data);
}

template<typename T>
FORCEINLINE typename Vector<T>::Iterator Vector<T>::end() noexcept
{
    return Iterator(m_Data + m_Length);
}

template<typename T>
FORCEINLINE typename Vector<T>::CIterator Vector<T>::cbegin() const noexcept
{
    return CIterator(m_Data);
}

template<typename T>
FORCEINLINE typename Vector<T>::CIterator Vector<T>::cend() const noexcept
{
    return CIterator(m_Data + m_Length);
}

template<typename T>
void swap(typename Vector<T>::Iterator& a, typename Vector<T>::Iterator& b) noexcept
{
    std::swap(*a, *b);
}

TRE_NS_END