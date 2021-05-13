#pragma once

#include <Core/Misc/Defines/Common.hpp>
#include <Core/Misc/Defines/Debug.hpp>
#include <Core/DataStructure/LinkedList/List/List.hpp>

TRE_NS_START

template<typename T, typename Container = List<T>>
class Stack
{
public:
	typedef typename Container::Iterator Iterator;
	typedef typename Container::CIterator CIterator;

	FORCEINLINE Stack();
	virtual ~Stack();

	template<typename... Args>
	FORCEINLINE T& Emplace(Args&&... args);

	FORCEINLINE T& Push(T& value);

	FORCEINLINE bool Pop();

	FORCEINLINE void Clear();

	FORCEINLINE T* Top() const;

	FORCEINLINE bool IsEmpty() const;

	Iterator begin() { return m_Stack.begin(); }
	Iterator end() { return m_Stack.end(); }

	const Iterator begin() const { return m_Stack.begin(); }
	const Iterator end() const { return m_Stack.end(); }

	const CIterator cbegin() const { return m_Stack.cbegin(); }
	const CIterator cend() const { return m_Stack.cend(); }
private:
	Container m_Stack;
};

template<typename T, typename Container>
FORCEINLINE Stack<T, Container>::Stack()
{
}

template<typename T, typename Container>
FORCEINLINE Stack<T, Container>::~Stack()
{
}

template<typename T, typename Container>
FORCEINLINE void Stack<T, Container>::Clear()
{
	m_Stack.Clear();
}

template<typename T, typename Container>
FORCEINLINE bool Stack<T, Container>::Pop()
{
	return m_Stack.PopBack();
}

template<typename T, typename Container>
template<typename... Args>
FORCEINLINE T& Stack<T, Container>::Emplace(Args&&... args)
{
	return m_Stack.EmplaceBack(std::forward<Args>(args)...);
}


template<typename T, typename Container>
FORCEINLINE T& Stack<T, Container>::Push(T& value)
{
	return m_Stack.PushBack(value);
}

template<typename T, typename Container>
FORCEINLINE T* Stack<T, Container>::Top() const
{
	return m_Stack.Back();
}

template<typename T, typename Container>
FORCEINLINE bool Stack<T, Container>::IsEmpty() const
{
	return m_Stack.IsEmpty();
}

template<typename T, typename Container = List<T>>
using LIFO = Stack<T, Container>;

TRE_NS_END

