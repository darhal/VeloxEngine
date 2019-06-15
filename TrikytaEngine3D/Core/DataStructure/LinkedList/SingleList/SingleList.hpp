#pragma once

#include <Core/Misc/Defines/Common.hpp>
#include <Core/Misc/Defines/Types.hpp>
#include <Core/Memory/Allocators/PoolAlloc/MultiPoolAllocator.hpp>

TRE_NS_START

template<typename T, typename Alloc_t = MultiPoolAlloc>
class SingleList
{
public:
	SingleList();
private:

	struct Node
	{
		template<typename... Args>
		Node(Node* next, Args&&... args) : m_Obj(std::forward<Args>(args)...), m_Next(next){
			if (m_Next != NULL) { m_Next->m_Previous = this; }
		}
		T m_Obj;
		Node* m_Next;
	};

	Node* m_Root;
	Alloc_t m_Allocator;
};

TRE_NS_END