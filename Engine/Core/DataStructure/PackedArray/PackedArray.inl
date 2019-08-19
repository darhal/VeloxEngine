
template<typename T, usize MAX_OBJECTS, typename ID_TYPE, typename INDEX_TYPE>
PackedArray<T, MAX_OBJECTS, ID_TYPE, INDEX_TYPE>::PackedArray() 
    : m_Objects(NULL), m_NumObjects(0),  m_FreelistEnqueue(MAX_OBJECTS - 1), m_FreelistDequeue(0)
{
	m_Objects = (Object*) ::operator new(sizeof(Object) * MAX_OBJECTS);

	for (ID i = 0; i < MAX_OBJECTS; ++i) {
		m_Indices[i].id = i;
		m_Indices[i].next = i+1;
		m_Indices[i].index = MAX;
	}	
}

template<typename T, usize MAX_OBJECTS, typename ID_TYPE, typename INDEX_TYPE>
template<typename... Args>
typename PackedArray<T, MAX_OBJECTS, ID_TYPE, INDEX_TYPE>::ID PackedArray<T, MAX_OBJECTS, ID_TYPE, INDEX_TYPE>::Emplace(Args&&... args) 
{
	Index& in = m_Indices[m_FreelistDequeue];
	m_FreelistDequeue = in.next;
	in.id += NEW_OBJECT_ID_ADD;
	in.index = (INDEX_TYPE) m_NumObjects++;
	new (&m_Objects[in.index]) Object(in.id, std::forward<Args>(args)...); 
	Object& o = m_Objects[in.index];
    // o.second = obj;  // Setup the object
	// o.first = in.id; // o.first instead of o.id
	return o.first;
}

template<typename T, usize MAX_OBJECTS, typename ID_TYPE, typename INDEX_TYPE>
typename PackedArray<T, MAX_OBJECTS, ID_TYPE, INDEX_TYPE>::ID PackedArray<T, MAX_OBJECTS, ID_TYPE, INDEX_TYPE>::Add(const T& obj) 
{
	Index& in = m_Indices[m_FreelistDequeue];
	m_FreelistDequeue = in.next;
	in.id += NEW_OBJECT_ID_ADD;
	in.index = m_NumObjects++;
	new (&m_Objects[in.index]) Object(in.id, obj); 
	Object& o = m_Objects[in.index];
    // o.second = obj;  // Setup the object
	// o.first = in.id; // o.first instead of o.id
	return o.first;
}

template<typename T, usize MAX_OBJECTS, typename ID_TYPE, typename INDEX_TYPE>
typename PackedArray<T, MAX_OBJECTS, ID_TYPE, INDEX_TYPE>::ID PackedArray<T, MAX_OBJECTS, ID_TYPE, INDEX_TYPE>::Add(T&& obj) 
{
	Index& in = m_Indices[m_FreelistDequeue];
	m_FreelistDequeue = in.next;
	in.id += NEW_OBJECT_ID_ADD;
	in.index = m_NumObjects++;
	new (&m_Objects[in.index]) Object(in.id, std::forward<T>(obj)); 
	Object& o = m_Objects[in.index];
    // o.second = obj;  // Setup the object
	// o.first = in.id; // o.first instead of o.id
	return o.first;
}

template<typename T, usize MAX_OBJECTS, typename ID_TYPE, typename INDEX_TYPE>
void PackedArray<T, MAX_OBJECTS, ID_TYPE, INDEX_TYPE>::Remove(ID id) 
{
	Index& in = m_Indices[id & INDEX_MASK];
		
	m_Objects[in.index].~Object(); // call dtor;
	new (&m_Objects[in.index]) Object(m_Objects[--m_NumObjects].first, std::move(m_Objects[--m_NumObjects].second));
	Object& o = m_Objects[in.index];

	// Object& o = m_Objects[in.index];
	// o = m_Objects[--m_NumObjects];
		
	m_Indices[o.first & INDEX_MASK].index = in.index; // o.first instead of o.id
		
	in.index = MAX;
	m_Indices[m_FreelistEnqueue].next = id & INDEX_MASK;
	m_FreelistEnqueue = id & INDEX_MASK;
}

template<typename T, usize MAX_OBJECTS, typename ID_TYPE, typename INDEX_TYPE>
FORCEINLINE bool PackedArray<T, MAX_OBJECTS, ID_TYPE, INDEX_TYPE>::Has(ID id) 
{
	Index& in = m_Indices[id & INDEX_MASK];
	return in.id == id && in.index != MAX;
}

template<typename T, usize MAX_OBJECTS, typename ID_TYPE, typename INDEX_TYPE>
FORCEINLINE typename PackedArray<T, MAX_OBJECTS, ID_TYPE, INDEX_TYPE>::Object& PackedArray<T, MAX_OBJECTS, ID_TYPE, INDEX_TYPE>::Lookup(ID id) 
{
	return m_Objects[m_Indices[id & INDEX_MASK].index];
}

template<typename T, usize MAX_OBJECTS, typename ID_TYPE, typename INDEX_TYPE>
FORCEINLINE INDEX_TYPE PackedArray<T, MAX_OBJECTS, ID_TYPE, INDEX_TYPE>::CompressID(ID id) const
{
	return INDEX_TYPE(id & INDEX_MASK);
}

template<typename T, usize MAX_OBJECTS, typename ID_TYPE, typename INDEX_TYPE>
FORCEINLINE const typename PackedArray<T, MAX_OBJECTS, ID_TYPE, INDEX_TYPE>::Iterator PackedArray<T, MAX_OBJECTS, ID_TYPE, INDEX_TYPE>::begin() const noexcept
{
	return Iterator(m_Objects);
}

template<typename T, usize MAX_OBJECTS, typename ID_TYPE, typename INDEX_TYPE>
FORCEINLINE const typename PackedArray<T, MAX_OBJECTS, ID_TYPE, INDEX_TYPE>::Iterator PackedArray<T, MAX_OBJECTS, ID_TYPE, INDEX_TYPE>::end() const noexcept
{
	return Iterator(m_Objects + m_NumObjects);
}

template<typename T, usize MAX_OBJECTS, typename ID_TYPE, typename INDEX_TYPE>
FORCEINLINE typename PackedArray<T, MAX_OBJECTS, ID_TYPE, INDEX_TYPE>::Iterator PackedArray<T, MAX_OBJECTS, ID_TYPE, INDEX_TYPE>::begin() noexcept
{
	return Iterator(m_Objects);
}

template<typename T, usize MAX_OBJECTS, typename ID_TYPE, typename INDEX_TYPE>
FORCEINLINE typename PackedArray<T, MAX_OBJECTS, ID_TYPE, INDEX_TYPE>::Iterator PackedArray<T, MAX_OBJECTS, ID_TYPE, INDEX_TYPE>::end() noexcept
{
	return Iterator(m_Objects + m_NumObjects);
}

template<typename T, usize MAX_OBJECTS, typename ID_TYPE, typename INDEX_TYPE>
FORCEINLINE typename PackedArray<T, MAX_OBJECTS, ID_TYPE, INDEX_TYPE>::CIterator PackedArray<T, MAX_OBJECTS, ID_TYPE, INDEX_TYPE>::cbegin() const noexcept
{
	return CIterator(m_Objects);
}

template<typename T, usize MAX_OBJECTS, typename ID_TYPE, typename INDEX_TYPE>
FORCEINLINE typename PackedArray<T, MAX_OBJECTS, ID_TYPE, INDEX_TYPE>::CIterator PackedArray<T, MAX_OBJECTS, ID_TYPE, INDEX_TYPE>::cend() const noexcept
{
	return CIterator(m_Objects + m_NumObjects);
}

template<typename T, usize MAX_OBJECTS, typename ID_TYPE, typename INDEX_TYPE>
PackedArray<T, MAX_OBJECTS, ID_TYPE, INDEX_TYPE>::~PackedArray()
{
	::operator delete(m_Objects);
}