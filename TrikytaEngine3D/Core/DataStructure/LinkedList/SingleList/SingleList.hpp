#pragma once

#include <Core/Misc/Defines/Common.hpp>
#include <Core/Misc/Defines/Types.hpp>
#include <Core/Memory/Allocators/PoolAlloc/MultiPoolAllocator.hpp>

TRE_NS_START

template<typename T, typename Alloc_t = MultiPoolAlloc>
class SingleList
{
public:
	class Iterator;

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

private:
	CONSTEXPR static usize BLOCK_NUM = 8;

	struct Node{
		template<typename... Args>
		Node(Node* next, Args&&... args) : m_Obj(std::forward<Args>(args)...), m_Next(next) {}
		T m_Obj;
		Node* m_Next;
	};

	Node* m_Head;
	Alloc_t m_Allocator;

	class Iterator
	{
	public:
		Iterator() noexcept : m_CurrentNode(m_Head) { }
		Iterator(Node* node) noexcept : m_CurrentNode(node) { }
		bool operator!=(const Iterator& iterator) { return m_CurrentNode != iterator.m_CurrentNode; }
		T& operator*() const { return const_cast<T&>(m_CurrentNode->m_Obj); }

		Iterator& operator=(Node* node)
		{
			this->m_CurrentNode = node;
			return *this;
		}

		Iterator& operator++()
		{
			if (m_CurrentNode)
				m_CurrentNode = m_CurrentNode->m_Next;
			return *this;
		}

		Iterator operator++(int)
		{
			Iterator iterator = *this;
			++*this;
			return iterator;
		}

	private:
		Node* m_CurrentNode;
	};

	template<typename U, typename A, typename std::enable_if<NO_DTOR(U), int>::type = 0>
	static void EmptyList(SingleList<U, A>& list);

	template<typename U, typename A, typename std::enable_if<HAVE_DTOR(U), int>::type = 0>
	static void EmptyList(SingleList<U, A>& list);

	template<typename U, typename A, typename std::enable_if<HAVE_DTOR(U), int>::type = 0>
	static void Destroy(SingleList<U, A>& list);

	template<typename U, typename A, typename std::enable_if<NO_DTOR(U), int>::type = 0>
	static void Destroy(SingleList<U, A>& list);
};

template<typename T, typename Alloc_t = MultiPoolAlloc>
using SList = SingleList<T, Alloc_t>;

#include "SingleList.inl"

TRE_NS_END

