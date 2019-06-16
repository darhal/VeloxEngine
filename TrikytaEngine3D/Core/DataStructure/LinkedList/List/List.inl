#include "List.hpp"

template<typename T, typename Alloc>
FORCEINLINE List<T, Alloc>::List() : m_Allocator(sizeof(Node), BLOCK_NUM, false)
{
}

template<typename T, typename Alloc>
FORCEINLINE List<T, Alloc>::List(usize nbChunk) : m_Allocator(sizeof(Node), nbChunk, false)
{
}

template<typename T, typename Alloc>
FORCEINLINE List<T, Alloc>::List(const std::initializer_list<T>& list) : m_Allocator(sizeof(Node), list.size())
{
	for (usize i = 0; i < list.size(); i++) {
		PushBack(list[i]);
	}
}

template<typename T, typename Alloc>
template<usize S>
List<T, Alloc>::List(const T(&objs_array)[S]) : m_Allocator(sizeof(Node), S)
{
	for (usize i = 0; i < S; i++) {
		PushBack(objs_array[i]);
	}
}

template<typename T, typename Alloc>
FORCEINLINE List<T, Alloc>::~List()
{
	List<T, Alloc>::Destroy(*this);
	// Allocator will go out of scope and free its memory by itself....
}

template<typename T, typename Alloc>
template<typename... Args>
FORCEINLINE T& List<T, Alloc>::EmplaceBack(Args&&... args)
{
	Node* node_ptr = (Node*) m_Allocator.Allocate(sizeof(Node));
	new (node_ptr) Node(m_Tail, NULL, std::forward<Args>(args)...);
	if (m_Head == NULL) m_Head = node_ptr;
	m_Tail = node_ptr;
	return node_ptr->m_Obj;
}

template<typename T, typename Alloc>
template<typename... Args>
FORCEINLINE T& List<T, Alloc>::EmplaceFront(Args&&... args)
{
	Node* node_ptr = (Node*)m_Allocator.Allocate(sizeof(Node));
	new (node_ptr) Node(NULL, m_Head, std::forward<Args>(args)...);
	if (m_Head == NULL) m_Tail = node_ptr;
	m_Head = node_ptr;
	return node_ptr->m_Obj;
}

template<typename T, typename Alloc>
FORCEINLINE T& List<T, Alloc>::PushBack(const T& obj)
{
	Node* node_ptr = (Node*)m_Allocator.Allocate(sizeof(Node));
	new (node_ptr) Node(m_Tail, obj);
	if (m_Head == NULL) m_Head = node_ptr;
	m_Tail = node_ptr;
	return node_ptr->m_Obj;
}

template<typename T, typename Alloc>
FORCEINLINE T& List<T, Alloc>::PushFront(const T& obj)
{
	Node* node_ptr = (Node*)m_Allocator.Allocate(sizeof(Node));
	new (node_ptr) Node(NULL, m_Head, obj);
	if (m_Head == NULL) m_Tail = node_ptr;
	m_Head = node_ptr;
	return node_ptr->m_Obj;
}

template<typename T, typename Alloc>
template<typename... Args>
FORCEINLINE T& List<T, Alloc>::Emplace(Iterator itr, Args&&... args)
{
	Node* node_ptr = (Node*)m_Allocator.Allocate(sizeof(Node));
	Node* node = itr->m_CurrentNode;
	new (node_ptr) Node(node->m_Previous ? node->m_Previous : NULL, node, std::forward<Args>(args)...);
	return node_ptr->m_Obj;
}

template<typename T, typename Alloc>
template<typename... Args>
FORCEINLINE T& List<T, Alloc>::Emplace(usize index, Args&&... args)
{
	auto head = m_Head;
	usize i = 0;
	while (head != NULL) {
		if (i == index) {
			return Emplace(head, std::forward<Args>(args)...);
		}
		head = head->m_Next;
		i++;
	}
}

template<typename T, typename Alloc>
FORCEINLINE T& List<T, Alloc>::Insert(usize index, const T& obj)
{
	auto head = m_Head;
	usize i = 0;
	while (head != NULL) {
		if (i == index) {
			return Insert(head, obj);
		}
		head = head->m_Next;
		i++;
	}
}

template<typename T, typename Alloc>
FORCEINLINE T& List<T, Alloc>::Insert(Iterator itr, const T& obj)
{
	Node* node_ptr = (Node*)m_Allocator.Allocate(sizeof(Node));
	Node* node = itr->m_CurrentNode;
	new (node_ptr) Node(node->m_Previous ? node->m_Previous  : NULL, node, obj);
	return node_ptr->m_Obj;
}

template<typename T, typename Alloc>
FORCEINLINE bool List<T, Alloc>::PopBack()
{
	if (m_Tail == NULL) return false;
	Node* tail_prev = m_Tail->m_Previous;
	//m_Tail->m_Obj.~T(); //Make sure to call the dtor for non pod types!
	m_Allocator.Deallocate(m_Tail);
	m_Tail = tail_prev;
	if (m_Tail != NULL)
		m_Tail->m_Next = NULL;
	return true;
}

template<typename T, typename Alloc>
FORCEINLINE bool List<T, Alloc>::PopFront()
{
	if (m_Head == NULL) return false;
	Node* head_next = m_Head->m_Next;
	//m_Head->m_Obj.~T();	 //Make sure to call the dtor for non pod types!
	m_Allocator.Deallocate(m_Head);
	m_Head = head_next;
	if (m_Head != NULL)
		m_Tail->m_Previous = NULL;
	return true;
}

template<typename T, typename Alloc>
FORCEINLINE bool List<T, Alloc>::IsEmpty() const
{
	return m_Head == NULL;
}

template<typename T, typename Alloc>
FORCEINLINE T* List<T, Alloc>::Front() const
{
	if (m_Head !=  NULL)
		return &(m_Head->m_Obj);
	return NULL;
}

template<typename T, typename Alloc>
FORCEINLINE T* List<T, Alloc>::Back() const
{
	if (m_Tail != NULL)
		return &(m_Tail->m_Obj);
	return NULL;
}

template <typename T, typename Alloc>
FORCEINLINE void List<T, Alloc>::Clear()
{
	List<T, Alloc>::EmptyList(*this);
}


template<typename U, typename A>
template<typename T, typename Alloc, typename std::enable_if<HAVE_DTOR(T), int>::type>
static void List<U, A>::EmptyList(List<T, Alloc>& list)
{
	auto head = list.m_Head;
	while (head != NULL) {
		head->m_Obj.~T(); //Make sure to call the dtor for non pod types!
		auto oldHead = head;
		head = head->m_Next;
		list.m_Allocator.Deallocate(oldHead);
	}
}

template<typename U, typename A>
template<typename T, typename Alloc, typename std::enable_if<NO_DTOR(T), int>::type>
static void List<U, A>::EmptyList(List<T, Alloc>& list)
{
	auto head = list.m_Head;
	while (head != NULL) {
		auto oldHead = head;
		head = head->m_Next;
		list.m_Allocator.Deallocate(oldHead);
	}
}

template<typename U, typename A>
template<typename T, typename Alloc, typename std::enable_if<HAVE_DTOR(T), int>::type>
static void List<U, A>::Destroy(List<T, Alloc>& list)
{
	auto head = list.m_Head;
	while (head != NULL) {
		head->m_Obj.~T(); //Make sure to call the dtor for non pod types!
		head = head->m_Next;
	}
}

template<typename U, typename A>
template<typename T, typename Alloc, typename std::enable_if<NO_DTOR(T), int>::type>
static void List<U, A>::Destroy(List<T, Alloc>& list)
{
}