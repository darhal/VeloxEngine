#pragma once

#include <Core/Misc/Defines/Common.hpp>
#include <Core/Misc/Defines/Debug.hpp>
#include <Core/DataStructure/LinkedList/List/List.hpp>

TRE_NS_START

template<typename T, typename Container = List<T>>
class Queue
{
public:
	FORCEINLINE Queue();

	virtual ~Queue();

	template<typename... Args>
	FORCEINLINE T& Emplace(Args&&... args);

	FORCEINLINE T& Push(T& value);

	FORCEINLINE bool Pop();

	FORCEINLINE void Clear();

	FORCEINLINE T* Front() const;

	FORCEINLINE bool IsEmpty() const;

private:
	Container m_Queue;
};


template<typename T, typename Container>
FORCEINLINE Queue<T, Container>::Queue()
{
}

template<typename T, typename Container>
FORCEINLINE Queue<T, Container>::~Queue()
{
}

template<typename T, typename Container>
template<typename ...Args>
FORCEINLINE T& Queue<T, Container>::Emplace(Args&&... args)
{
	return m_Queue.EmplaceBack(std::forward<Args>(args)...);
}

template<typename T, typename Container>
FORCEINLINE T& Queue<T, Container>::Push(T& value)
{
	m_Queue.PushBack(value);
}

template<typename T, typename Container>
FORCEINLINE T* Queue<T, Container>::Front() const
{
	return m_Queue.Front();
}

template<typename T, typename Container>
FORCEINLINE bool Queue<T, Container>::Pop()
{
	return m_Queue.PopFront();
}

template<typename T, typename Container>
FORCEINLINE void Queue<T, Container>::Clear()
{
	m_Queue.Clear();
}

template<typename T, typename Container>
FORCEINLINE bool Queue<T, Container>::IsEmpty() const
{
	return m_Queue.IsEmpty();
}


template<typename T, typename Container = List<T>>
using FIFO = Queue<T, Container>;

TRE_NS_END

