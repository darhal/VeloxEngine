template<typename T, typename Alloc_t>
FORCEINLINE SingleList<T, Alloc_t>::SingleList() : m_Head(NULL), m_Allocator(sizeof(Node), BLOCK_NUM, false)
{
}

template<typename T, typename Alloc_t>
FORCEINLINE SingleList<T, Alloc_t>::~SingleList()
{
	SingleList<T, Alloc_t>::Destroy(*this);
}

template<typename T, typename Alloc_t>
template<typename ...Args>
FORCEINLINE T& SingleList<T, Alloc_t>::EmplaceFront(Args&& ...args)
{
	Node* node_ptr = (Node*) m_Allocator.Allocate(sizeof(Node));
	new (node_ptr) Node(m_Head, ::std::forward<Args>(args)...);
	m_Head = node_ptr;
	return node_ptr->m_Obj;
}

template<typename T, typename Alloc_t>
FORCEINLINE T& SingleList<T, Alloc_t>::PushFront(const T & obj)
{
	Node* node_ptr = (Node*) m_Allocator.Allocate(sizeof(Node));
	new (node_ptr) Node(m_Head, obj);
	m_Head = node_ptr;
	return node_ptr->m_Obj;
}

template<typename T, typename Alloc_t>
FORCEINLINE bool SingleList<T, Alloc_t>::PopFront()
{
	if (m_Head == NULL) 
		return false;

	Node* head_next = m_Head->m_Next;
	m_Allocator.Deallocate(m_Head);
	m_Head = head_next;
	return true;
}

template<typename T, typename Alloc_t>
FORCEINLINE T* SingleList<T, Alloc_t>::Front() const
{
	if (m_Head == NULL) 
		return NULL;
	
	return &(m_Head->m_Obj);
}

template<typename T, typename Alloc_t>
FORCEINLINE T* SingleList<T, Alloc_t>::Back() const
{
	Node* current = m_Head;

	if (current == NULL) 
		return NULL;

	while (current != NULL  && current->m_Next != NULL) {
		current = current->m_Next;
	}

	return &(current->m_Obj);
}

template<typename T, typename Alloc_t>
FORCEINLINE void SingleList<T, Alloc_t>::Clear()
{
	SingleList<T, Alloc_t>::EmptyList(*this);
}

template<typename T, typename Alloc_t>
FORCEINLINE bool SingleList<T, Alloc_t>::IsEmpty() const
{
	return m_Head == NULL;
}

template<typename U, typename A>
template<typename T, typename Alloc, typename ::std::enable_if<HAVE_DTOR(T), int>::type>
void SingleList<U, A>::EmptyList(SingleList<T, Alloc>& list)
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
template<typename T, typename Alloc, typename ::std::enable_if<NO_DTOR(T), int>::type>
void SingleList<U, A>::EmptyList(SingleList<T, Alloc>& list)
{
	auto head = list.m_Head;

	while (head != NULL) {
		auto oldHead = head;
		head = head->m_Next;
		list.m_Allocator.Deallocate(oldHead);
	}
}

template<typename U, typename A>
template<typename T, typename Alloc, typename ::std::enable_if<HAVE_DTOR(T), int>::type>
void SingleList<U, A>::Destroy(SingleList<T, Alloc>& list)
{
	auto head = list.m_Head;
	
	while (head != NULL) {
		head->m_Obj.~T(); //Make sure to call the dtor for non pod types!
		head = head->m_Next;
	}
}

template<typename U, typename A>
template<typename T, typename Alloc, typename ::std::enable_if<NO_DTOR(T), int>::type>
void SingleList<U, A>::Destroy(SingleList<T, Alloc>& list)
{
}
