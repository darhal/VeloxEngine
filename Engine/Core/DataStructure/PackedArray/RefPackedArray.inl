
template<typename T, usize MAX_OBJECTS, typename ID_TYPE, typename INDEX_TYPE>
RefCounterPackedArray<T, MAX_OBJECTS, ID_TYPE, INDEX_TYPE>::RefCounterPackedArray() 
    : m_Objects(NULL), m_NumObjects(0),  m_FreelistEnqueue(MAX_OBJECTS - 1), m_FreelistDequeue(0)
{
	m_Objects = (Object*) ::operator new(sizeof(Object) * MAX_OBJECTS);

	for (ID i = 0; i < MAX_OBJECTS; ++i) {
		m_Indices[i].id = i;
		m_Indices[i].next = i+1;
		m_Indices[i].index = MAX;
        m_Indices[i].count = 0;
	}	
}

template<typename T, usize MAX_OBJECTS, typename ID_TYPE, typename INDEX_TYPE>
template<typename... Args>
typename RefCounterPackedArray<T, MAX_OBJECTS, ID_TYPE, INDEX_TYPE>::ID RefCounterPackedArray<T, MAX_OBJECTS, ID_TYPE, INDEX_TYPE>::Emplace(Args&&... args) 
{
	Index& in = m_Indices[m_FreelistDequeue];
	m_FreelistDequeue = in.next;
	in.id += NEW_OBJECT_ID_ADD;
	in.index = m_NumObjects++;
    in.count = 1;
	new (&m_Objects[in.index]) Object(in.id, std::forward<Args>(args)...); 
	Object& o = m_Objects[in.index];
    // o.second = obj;  // Setup the object
	// o.first = in.id; // o.first instead of o.id
	return o.first;
}

template<typename T, usize MAX_OBJECTS, typename ID_TYPE, typename INDEX_TYPE>
typename RefCounterPackedArray<T, MAX_OBJECTS, ID_TYPE, INDEX_TYPE>::ID RefCounterPackedArray<T, MAX_OBJECTS, ID_TYPE, INDEX_TYPE>::Add(const T& obj) 
{
	Index& in = m_Indices[m_FreelistDequeue];
	m_FreelistDequeue = in.next;
	in.id += NEW_OBJECT_ID_ADD;
	in.index = m_NumObjects++;
	in.count = 1;
	new (&m_Objects[in.index]) Object(in.id, obj); 
	Object& o = m_Objects[in.index];
    // o.second = obj;  // Setup the object
	// o.first = in.id; // o.first instead of o.id
	return o.first;
}

template<typename T, usize MAX_OBJECTS, typename ID_TYPE, typename INDEX_TYPE>
typename RefCounterPackedArray<T, MAX_OBJECTS, ID_TYPE, INDEX_TYPE>::ID RefCounterPackedArray<T, MAX_OBJECTS, ID_TYPE, INDEX_TYPE>::Add(T&& obj) 
{
	Index& in = m_Indices[m_FreelistDequeue];
	m_FreelistDequeue = in.next;
	in.id += NEW_OBJECT_ID_ADD;
	in.index = m_NumObjects++;
	in.count = 1;
	new (&m_Objects[in.index]) Object(in.id, std::forward<T>(obj)); 
	Object& o = m_Objects[in.index];
    // o.second = obj;  // Setup the object
	// o.first = in.id; // o.first instead of o.id
	return o.first;
}

template<typename T, usize MAX_OBJECTS, typename ID_TYPE, typename INDEX_TYPE>
void RefCounterPackedArray<T, MAX_OBJECTS, ID_TYPE, INDEX_TYPE>::Remove(ID id) 
{
	Index& in = m_Indices[id & INDEX_MASK];
	in.count -= 1;

	if (in.count)
		return;

	m_Objects[in.index].~Object(); // call dtor;
	new (&m_Objects[in.index]) Object(m_Objects[--m_NumObjects].first, std::move(m_Objects[--m_NumObjects].second));
	Object& o = m_Objects[in.index];
		
	m_Indices[o.first & INDEX_MASK].index = in.index; // o.first instead of o.id
		
	in.index = MAX;
	m_Indices[m_FreelistEnqueue].next = id & INDEX_MASK;
	m_FreelistEnqueue = id & INDEX_MASK;
}

template<typename T, usize MAX_OBJECTS, typename ID_TYPE, typename INDEX_TYPE>
T* RefCounterPackedArray<T, MAX_OBJECTS, ID_TYPE, INDEX_TYPE>::AddReference(ID id)
{
	Index& in = m_Indices[id & INDEX_MASK];

	if (!in.count || in.id != id || in.index == MAX)
		return NULL;

	in.count += 1;
	return &m_Objects[in.index];
}

template<typename T, usize MAX_OBJECTS, typename ID_TYPE, typename INDEX_TYPE>
INDEX_TYPE RefCounterPackedArray<T, MAX_OBJECTS, ID_TYPE, INDEX_TYPE>::GetRefCount(ID id) const
{
	Index& in = m_Indices[id & INDEX_MASK];
	return in.count;
}

template<typename T, usize MAX_OBJECTS, typename ID_TYPE, typename INDEX_TYPE>
FORCEINLINE bool RefCounterPackedArray<T, MAX_OBJECTS, ID_TYPE, INDEX_TYPE>::Has(ID id) 
{
	Index& in = m_Indices[id & INDEX_MASK];
	return in.id == id && in.index != MAX;
}

template<typename T, usize MAX_OBJECTS, typename ID_TYPE, typename INDEX_TYPE>
FORCEINLINE typename RefCounterPackedArray<T, MAX_OBJECTS, ID_TYPE, INDEX_TYPE>::Object& RefCounterPackedArray<T, MAX_OBJECTS, ID_TYPE, INDEX_TYPE>::Lookup(ID id) 
{
	return m_Objects[m_Indices[id & INDEX_MASK].index];
}

template<typename T, usize MAX_OBJECTS, typename ID_TYPE, typename INDEX_TYPE>
FORCEINLINE INDEX_TYPE RefCounterPackedArray<T, MAX_OBJECTS, ID_TYPE, INDEX_TYPE>::CompressID(ID id) const
{
	return INDEX_TYPE(id & INDEX_MASK);
}

template<typename T, usize MAX_OBJECTS, typename ID_TYPE, typename INDEX_TYPE>
FORCEINLINE const typename RefCounterPackedArray<T, MAX_OBJECTS, ID_TYPE, INDEX_TYPE>::Iterator RefCounterPackedArray<T, MAX_OBJECTS, ID_TYPE, INDEX_TYPE>::begin() const noexcept
{
	return Iterator(m_Objects);
}

template<typename T, usize MAX_OBJECTS, typename ID_TYPE, typename INDEX_TYPE>
FORCEINLINE const typename RefCounterPackedArray<T, MAX_OBJECTS, ID_TYPE, INDEX_TYPE>::Iterator RefCounterPackedArray<T, MAX_OBJECTS, ID_TYPE, INDEX_TYPE>::end() const noexcept
{
	return Iterator(m_Objects + m_NumObjects);
}

template<typename T, usize MAX_OBJECTS, typename ID_TYPE, typename INDEX_TYPE>
FORCEINLINE typename RefCounterPackedArray<T, MAX_OBJECTS, ID_TYPE, INDEX_TYPE>::Iterator RefCounterPackedArray<T, MAX_OBJECTS, ID_TYPE, INDEX_TYPE>::begin() noexcept
{
	return Iterator(m_Objects);
}

template<typename T, usize MAX_OBJECTS, typename ID_TYPE, typename INDEX_TYPE>
FORCEINLINE typename RefCounterPackedArray<T, MAX_OBJECTS, ID_TYPE, INDEX_TYPE>::Iterator RefCounterPackedArray<T, MAX_OBJECTS, ID_TYPE, INDEX_TYPE>::end() noexcept
{
	return Iterator(m_Objects + m_NumObjects);
}

template<typename T, usize MAX_OBJECTS, typename ID_TYPE, typename INDEX_TYPE>
FORCEINLINE typename RefCounterPackedArray<T, MAX_OBJECTS, ID_TYPE, INDEX_TYPE>::CIterator RefCounterPackedArray<T, MAX_OBJECTS, ID_TYPE, INDEX_TYPE>::cbegin() const noexcept
{
	return CIterator(m_Objects);
}

template<typename T, usize MAX_OBJECTS, typename ID_TYPE, typename INDEX_TYPE>
FORCEINLINE typename RefCounterPackedArray<T, MAX_OBJECTS, ID_TYPE, INDEX_TYPE>::CIterator RefCounterPackedArray<T, MAX_OBJECTS, ID_TYPE, INDEX_TYPE>::cend() const noexcept
{
	return CIterator(m_Objects + m_NumObjects);
}

template<typename T, usize MAX_OBJECTS, typename ID_TYPE, typename INDEX_TYPE>
RefCounterPackedArray<T, MAX_OBJECTS, ID_TYPE, INDEX_TYPE>::~RefCounterPackedArray()
{
	::operator delete(m_Objects);
}