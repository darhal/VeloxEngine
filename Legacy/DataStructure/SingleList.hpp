#pragma once

#include <Legacy/Misc/Defines/Common.hpp>
#include <Legacy/Misc/Defines/Types.hpp>
#include <Legacy/Memory/Allocators/PoolAlloc/MultiPoolAllocator.hpp>

TRE_NS_START

template<typename T, typename Alloc_t = MultiPoolAlloc>
class SingleList
{
public:
	template<typename DataType>
	class GIterator;

	typedef GIterator<Node> Iterator;
	typedef GIterator<const Node> CIterator;


	struct Node{
		template<typename... Args>
		Node(Node* next, Args&&... args) : 
			m_Obj(std::forward<Args>(args)...), m_Next(next) {}

		T m_Obj;
		Node* m_Next;
	};

public:
	SingleList();

	FORCEINLINE ~SingleList();

	template<typename... Args>
	FORCEINLINE T& EmplaceFront(Args&&... args);
	FORCEINLINE T& PushFront(const T& obj);

	FORCEINLINE bool PopFront();

	FORCEINLINE bool IsEmpty() const;

	FORCEINLINE T* Front() const;
	FORCEINLINE T* Back() const;
	FORCEINLINE void Clear();

	Iterator begin() { return Iterator(m_Head); }
	Iterator end() { return Iterator(NULL); }

	const Iterator begin() const { return Iterator(m_Head); }
	const Iterator end() const { return Iterator(NULL); }

	const CIterator cbegin() const { return CIterator(m_Head); }
	const CIterator cend() const { return CIterator(NULL); }

private:

	CONSTEXPR static usize BLOCK_NUM = 8;

	Node* m_Head;
	Alloc_t m_Allocator;

	template<typename U, typename A, typename std::enable_if<NO_DTOR(U), int>::type = 0>
	static void EmptyList(SingleList<U, A>& list);

	template<typename U, typename A, typename std::enable_if<HAVE_DTOR(U), int>::type = 0>
	static void EmptyList(SingleList<U, A>& list);

	template<typename U, typename A, typename std::enable_if<HAVE_DTOR(U), int>::type = 0>
	static void Destroy(SingleList<U, A>& list);

	template<typename U, typename A, typename std::enable_if<NO_DTOR(U), int>::type = 0>
	static void Destroy(SingleList<U, A>& list);

public:

	template<typename DataType>
	class GIterator
	{
	public:
		GIterator() noexcept : m_CurrentNode(m_Head) { }
		GIterator(DataType* node) noexcept : m_CurrentNode(node) { }
		bool operator!=(const Iterator& iterator) { return m_CurrentNode != iterator.m_CurrentNode; }
		
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

	private:
		DataType* m_CurrentNode;
	};
};

template<typename T, typename Alloc_t = MultiPoolAlloc>
using SList = SingleList<T, Alloc_t>;

#include "SingleList.inl"

TRE_NS_END

