#pragma once

#include <Core/Misc/Defines/Common.hpp>
#include <Core/Misc/Defines/Types.hpp>
#include <Core/Memory/Allocators/PoolAlloc/MultiPoolAllocator.hpp>

TRE_NS_START

template<typename T, typename Alloc_t = MultiPoolAlloc>
class List
{
public:
	template<typename DataType>
	class GIterator;

	struct Node;

	typedef GIterator<Node> Iterator;
	typedef GIterator<const Node> CIterator;

public:
	FORCEINLINE List();
	FORCEINLINE List(usize nbChunk);
	FORCEINLINE List(const std::initializer_list<T>& list);
	template<usize S>
	FORCEINLINE List(const T(&objs_array)[S]);

	FORCEINLINE ~List();

	template<typename... Args>
	FORCEINLINE T& EmplaceBack(Args&&... args);
	template<typename... Args>
	FORCEINLINE T& EmplaceFront(Args&&... args);

	FORCEINLINE T& PushBack(const T& obj);
	FORCEINLINE T& PushFront(const T& obj);

	FORCEINLINE T& Insert(usize index, const T& obj);
	FORCEINLINE T& Insert(Iterator itr, const T& obj);

	template<typename... Args>
	FORCEINLINE T& Emplace(Iterator itr, Args&&... args);
	template<typename... Args>
	FORCEINLINE T& Emplace(usize index, Args&&... args);

	FORCEINLINE bool PopBack();
	FORCEINLINE bool PopFront();

	FORCEINLINE bool IsEmpty() const;

	FORCEINLINE T* Front() const;
	FORCEINLINE T* Back() const;
	FORCEINLINE void Clear();

	Iterator begin() { return Iterator(m_Head); }
	Iterator end() { return Iterator(m_Tail); }

	const Iterator begin() const { return Iterator(m_Head); }
	const Iterator end() const { return Iterator(m_Tail); }

	const CIterator cbegin() const { return CIterator(m_Head); }
	const CIterator cend() const { return CIterator(m_Tail); }

private:
	CONSTEXPR static usize BLOCK_NUM = 8;

	struct Node
	{
		template<typename... Args>
		Node(Node* prev, Node* next, Args&&... args) : m_Obj(std::forward<Args>(args)...), m_Previous(prev), m_Next(next) 
		{
			if (m_Previous != NULL) {m_Previous->m_Next = this;}
			if (m_Next != NULL) {m_Next->m_Previous = this;}
		}
		T m_Obj;
		Node* m_Next;
		Node* m_Previous;
	};

	Node* m_Head;
	Node* m_Tail;
	Alloc_t m_Allocator;

	template<typename DataType>
	class GIterator : public std::iterator<std::bidirectional_iterator_tag, DataType, ptrdiff_t, DataType*, DataType&>
	{
	public:
		GIterator() noexcept : m_CurrentNode(m_Head) { }
		GIterator(DataType* node) noexcept : m_CurrentNode(node) { }
		bool operator!=(const GIterator& iterator) { return m_CurrentNode != iterator.m_CurrentNode; }

		DataType& operator*() { return *m_CurrentNode; }
		const DataType& operator*() const { return (*m_CurrentNode); }

		DataType* operator->() { return m_CurrentNode; }
		const DataType* operator->() const { return m_CurrentNode; }

		GIterator& operator=(DataType* node)
		{
			this->m_CurrentNode = node;
			return *this;
		}

		GIterator& operator++()
		{
			if (m_CurrentNode)
				m_CurrentNode = m_CurrentNode->m_Next;
			return *this;
		}

		GIterator operator++(int)
		{
			GIterator iterator = *this;
			++*this;
			return iterator;
		}

		GIterator& operator--()
		{
			if (m_CurrentNode)
				m_CurrentNode = m_CurrentNode->m_Previous;
			return *this;
		}

		GIterator operator--(int)
		{
			GIterator iterator = *this;
			--*this;
			return iterator;
		}

	private:
		DataType* m_CurrentNode;
	};

	template<typename U, typename A, typename std::enable_if<NO_DTOR(U), int>::type = 0>
	static void EmptyList(List<U, A>& list);

	template<typename U, typename A, typename std::enable_if<HAVE_DTOR(U), int>::type = 0>
	static void EmptyList(List<U, A>& list);

	template<typename U, typename A, typename std::enable_if<HAVE_DTOR(U), int>::type = 0>
	static void Destroy(List<U, A>& list);

	template<typename U, typename A, typename std::enable_if<NO_DTOR(U), int>::type = 0>
	static void Destroy(List<U, A>& list);
};

template<typename T, typename Alloc_t = MultiPoolAlloc>
using DoubleList = List<T, Alloc_t>;

template<typename T, typename Alloc_t = MultiPoolAlloc>
using DList = List<T, Alloc_t>;

#include "List.inl"

TRE_NS_END