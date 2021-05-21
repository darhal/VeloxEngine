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
    Vector() noexcept;

    Vector(usize sz) noexcept;

    Vector(usize sz, const T& obj);
    
    // Vector(const std::initializer_list<T>& list);

    template<usize S>
    Vector(T(&arr)[S]);

    Vector(T* data, usize size);

    ~Vector();

    FORCEINLINE bool Reserve(usize sz);

    template<typename... Args>
    T& EmplaceBack(Args&&... args);

    T& PushBack(const T& obj);

    template<typename... Args>
    T& EmplaceFront(Args&&... args);

    T& PushFront(const T& obj);

    template<typename... Args>
    T& EmplaceFrontFast(Args&&... args);

    T& PushFrontFast(const T& obj);

    T& Insert(usize i, const T& obj);

    template<typename... Args>
    T& Emplace(usize i, Args&&... args);

    T& InsertFast(usize i, const T& obj);

    template<typename... Args>
    T& EmplaceFast(usize i, Args&&... args);

    void Erease(usize start, usize end) noexcept;

    void Erease(Iterator itr) noexcept;

    void Erease(usize index) noexcept;

    void EreaseFast(Iterator itr) noexcept;

    void EreaseFast(usize index) noexcept;

    void Clear() noexcept;

    bool PopBack() noexcept;

    bool PopFront() noexcept;

    bool PopFrontFast() noexcept;

    void Fill(const T& obj, usize length);

    void Resize(usize newSize);

    bool IsEmpty() const noexcept;

    usize Capacity() const noexcept;

    usize Length() const noexcept;

    usize Size() const noexcept;

    T* Back() const noexcept;

    T* Front() const noexcept;

    T* Data() const noexcept { return this->Front(); };

    void Append(const Vector<T>& other);

    void Append(Vector<T>&& other);

    T& Get(usize i) noexcept;

    T& At(usize i) noexcept;

    T& operator[](usize i) noexcept;

    const T& At(usize i) const noexcept;

    const T& operator[](usize i) const noexcept;

    Iterator begin() noexcept;

    Iterator end() noexcept;

    const Iterator begin() const noexcept;

    const Iterator end() const noexcept;

    CIterator cbegin() const noexcept;

    CIterator cend() const noexcept;

    Vector(const Vector<T>& other);

    Vector& operator=(const Vector<T>& other);

    Vector(Vector<T>&& other) noexcept;

    Vector& operator=(Vector<T>&& other) noexcept;

    Vector& operator+=(const Vector<T>& other);

    Vector& operator+=(Vector<T>&& other);

    T* StealPtr() noexcept;

    friend void Swap(Vector<T>& first, Vector<T>& second) noexcept
    {
        std::swap(first.m_Data, second.m_Data);
        std::swap(first.m_Length, second.m_Length);
        std::swap(first.m_Capacity, second.m_Capacity);
    }

    friend void swap(Vector<T>& first, Vector<T>& second) noexcept
    {
        Swap(first, second);
    }
private:
    FORCEINLINE void Reallocate(usize nCap);

    FORCEINLINE bool Allocate(usize cap = 0);

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
Vector<T>::Vector() noexcept 
    : m_Data(NULL), m_Length(0), m_Capacity(DEFAULT_CAPACITY)
{
}

template<typename T>
Vector<T>::Vector(usize sz) noexcept 
    : m_Data(NULL), m_Length(0), m_Capacity(sz)
{
}

template<typename T>
Vector<T>::Vector(usize sz, const T& obj) 
    : m_Data(NULL), m_Length(0), m_Capacity(sz)
{
    this->Fill(obj, m_Capacity);
}

template<typename T>
template<usize S>
Vector<T>::Vector(T(&arr)[S]) 
    : m_Data(NULL), m_Length(S), m_Capacity(S)
{
    m_Data = Utils::Allocate<T>(m_Capacity);
    Utils::CopyConstruct(m_Data, arr, m_Length);
}

template<typename T>
Vector<T>::Vector(T* data, usize size) 
    : m_Data(NULL), m_Length(size), m_Capacity(size)
{
    m_Data = Utils::Allocate<T>(m_Capacity);
    Utils::CopyConstruct(m_Data, data, m_Length);
}

/*template<typename T>
Vector<T>::Vector(const std::initializer_list<T>& list) 
    : m_Data(NULL), m_Length(list.size()), m_Capacity(list.size())
{
    m_Data = Utils::Allocate<T>(m_Capacity);
    T* ptr = m_Data;

    for (const T& obj : list) {
        new (ptr) T(obj);
        ptr++;
    }
}*/

template<typename T>
Vector<T>::~Vector()
{
    if (m_Data != NULL) {
        Utils::Free(m_Data, m_Length);
        m_Data = NULL;
    }
}

template<typename T>
void Vector<T>::Fill(const T& obj, usize length)
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
T& Vector<T>::PushBack(const T& obj)
{
    return this->EmplaceBack(obj);
}

template<typename T>
bool Vector<T>::PopBack() noexcept
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
    if (this->Allocate(sz)) {
        return true;
    }

    if (sz < m_Capacity)
        return false;

    this->Reallocate(sz * DEFAULT_GROW_SIZE);
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
FORCEINLINE bool Vector<T>::Allocate(usize cap)
{
    if (m_Data == NULL) {
        m_Capacity = cap > m_Capacity ? cap : m_Capacity;
        m_Data = Utils::Allocate<T>(m_Capacity);
        return true;
    }

    return false;
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
T& Vector<T>::Insert(usize i, const T& obj)
{
    return this->Emplace(i, obj);
}

template<typename T>
T& Vector<T>::PushFront(const T& obj)
{
    return this->Insert(0, obj);
}

template<typename T>
template<typename... Args>
T& Vector<T>::EmplaceFrontFast(Args&&... args)
{
    return this->EmplaceFast(0, std::forward<Args>(args)...);
}

template<typename T>
T& Vector<T>::PushFrontFast(const T& obj)
{
    return this->InsertFast(0, obj);
}

template<typename T>
template<typename... Args>
T& Vector<T>::Emplace(usize i, Args&&... args)
{
    ASSERTF(i > m_Length, "Given index is out of bound please choose from [0..%" SZu "].", m_Length);

    if (m_Length + 1 >= m_Capacity) {
        usize nCap = m_Capacity * DEFAULT_GROW_SIZE;
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
        this->Allocate();
        T* dest = m_Data + i;
        // shift all of this to keep place for the new element
        Utils::MoveConstructBackward(m_Data + 1, m_Data, m_Length - 1, i);
        new (dest) T(std::forward<Args>(args)...);
        m_Length++;
        return *(dest);
    }
}

template<typename T>
T& Vector<T>::InsertFast(usize i, const T& obj)
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

    this->Allocate();
    T* element = m_Data + i;
    
    if (i >= m_Length) {
        new (element) T(std::forward<Args>(args)...);
    } else {
        T* last = m_Data + m_Length;
        T temp(std::move(*element));
        new (element) T(std::forward<Args>(args)...);
        new (last) T(std::move(temp));
    }

    m_Length++;
    return *element;
}

template<typename T>
template<typename ...Args>
T& Vector<T>::EmplaceFront(Args&&... args)
{
    return this->Emplace(0, std::forward<Args>(args)...);
}

template<typename T>
void Vector<T>::Append(const Vector<T>& other)
{
    this->Reserve(m_Length + other.m_Length);
    Utils::Copy(m_Data + m_Length, other.m_Data, other.m_Length);
    m_Length += other.m_Length;
}

template<typename T>
void Vector<T>::Append(Vector<T>&& other)
{
    this->Reserve(m_Length + other.m_Length);
    Utils::Move(m_Data + m_Length, other.m_Data, other.m_Length);
    m_Length += other.m_Length;
    other.m_Length = 0;
}

template<typename T>
Vector<T>& Vector<T>::operator+=(const Vector<T>& other)
{
    this->Append(other);
    return *this;
}

template<typename T>
Vector<T>& Vector<T>::operator+=(Vector<T>&& other)
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
void Vector<T>::Erease(usize index) noexcept
{
    return this->Erease(this->begin() + index);
}

template<typename T>
void Vector<T>::EreaseFast(Iterator itr) noexcept
{
    T* itr_ptr = itr.GetPtr();
    ASSERTF((itr_ptr >= m_Data + m_Length || itr_ptr < m_Data), "The given iterator doesn't belong to the Vector.");
    
    (*itr_ptr).~T();
    T* last_ptr = m_Data + m_Length - 1;
    new (itr_ptr) T(std::move(*last_ptr));
    m_Length -= 1;
}

template<typename T>
void Vector<T>::EreaseFast(usize index) noexcept
{
    return this->EreaseFast(this->begin() + index);
}

template<typename T>
T* Vector<T>::StealPtr() noexcept
{
    T* data_ptr = m_Data;
    m_Length = 0;
    m_Capacity = 0;
    m_Data = NULL;
    return data_ptr;
}

template<typename T>
void Vector<T>::Clear() noexcept
{
    Utils::Destroy(m_Data, m_Length);
    m_Length = 0;
}

template<typename T>
bool Vector<T>::IsEmpty() const noexcept
{
    return this->Size() == 0;
}

template<typename T>
usize Vector<T>::Capacity() const noexcept
{
    return m_Capacity;
}

template<typename T>
usize Vector<T>::Length() const noexcept
{
    return m_Length;
}

template<typename T>
usize Vector<T>::Size() const noexcept
{
    return m_Length;
}

template<typename T>
T* Vector<T>::Back() const noexcept
{
    if (m_Length == 0)
        return NULL;

    return m_Data + m_Length - 1;
}

template<typename T>
T* Vector<T>::Front() const noexcept
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
T& Vector<T>::Get(usize i) noexcept
{
    return At(i);
}

template<typename T>
T& Vector<T>::At(usize i) noexcept
{
    ASSERTF((i >= m_Length), "Bad usage of vector function At index out of bounds");
    return m_Data[i];
}

template<typename T>
T& Vector<T>::operator[](usize i) noexcept
{
    return At(i);
}

template<typename T>
const T& Vector<T>::At(usize i) const noexcept
{
    ASSERTF((i >= m_Length), "Bad usage of vector function At index out of bounds");
    return m_Data[i];
}

template<typename T>
const T& Vector<T>::operator[](usize i) const noexcept
{
    return this->At(i);
}

template<typename T>
Vector<T>::Vector(const Vector<T>& other) : 
    m_Data(nullptr), m_Length(other.m_Length), m_Capacity(other.m_Capacity)
{
    if (m_Length) {
        m_Data = Utils::Allocate<T>(m_Length);
        Utils::Copy(other.m_Data, m_Data, m_Length);
    }
}

template<typename T>
Vector<T>& Vector<T>::operator=(const Vector<T>& other)
{
    Vector<T> tmp(other);
    Swap(*this, tmp);
    return *this;
}

template<typename T>
Vector<T>::Vector(Vector<T>&& other) noexcept 
    : m_Data(other.m_Data), m_Length(other.m_Length), m_Capacity(other.m_Capacity)
{
    other.m_Data = NULL;
}

template<typename T>
Vector<T>& Vector<T>::operator=(Vector<T>&& other) noexcept
{
    Vector<T> tmp(std::move(other));
    Swap(*this, tmp);
    return *this;
}

template<typename T>
const typename Vector<T>::Iterator Vector<T>::begin() const noexcept
{
    return Iterator(m_Data);
}

template<typename T>
const typename Vector<T>::Iterator Vector<T>::end() const noexcept
{
    return Iterator(m_Data + m_Length);
}

template<typename T>
typename Vector<T>::Iterator Vector<T>::begin() noexcept
{
    return Iterator(m_Data);
}

template<typename T>
typename Vector<T>::Iterator Vector<T>::end() noexcept
{
    return Iterator(m_Data + m_Length);
}

template<typename T>
typename Vector<T>::CIterator Vector<T>::cbegin() const noexcept
{
    return CIterator(m_Data);
}

template<typename T>
typename Vector<T>::CIterator Vector<T>::cend() const noexcept
{
    return CIterator(m_Data + m_Length);
}

template<typename T>
void swap(typename Vector<T>::Iterator& a, typename Vector<T>::Iterator& b) noexcept
{
    std::swap(*a, *b);
}

TRE_NS_END