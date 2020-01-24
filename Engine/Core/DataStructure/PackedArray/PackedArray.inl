#include <Core/Memory/Utils/Utils.hpp>

template<typename T>
PackedArray<T>::PackedArray(uint32 capacity) : 
	m_Objects(NULL), m_Capacity(capacity), m_ObjectCount(0), 
	m_FreelistEnqueue(m_Capacity - 1), m_FreelistDequeue(0)
{
	void* data = Allocate<uint8>((sizeof(Pair<ID, T>) + sizeof(Index)) * m_Capacity);
	m_Objects = (Pair<ID, T>*) data;
	Index* indices = this->GetIndexArray();

	for (uint32 i = 0; i < m_Capacity; i++) {
		indices[i].index = INVALID_INDEX;
		indices[i].next_free = i + 1;
	}
}

template<typename T>
typename PackedArray<T>::ID PackedArray<T>::Add(const T& object)
{
	const Pair<typename PackedArray<T>::ID, T>& pair = this->Emplace(object);
	return pair.first;
}

template<typename T>
void PackedArray<T>::Remove(ID id)
{
	Index* indices = this->GetIndexArray();
	Index& in = indices[id]; // Get index
	Pair<ID, T>& object_to_delete = m_Objects[in.index]; // Get Object to delete
	object_to_delete.second.~T(); // Call dtor

	Pair<ID, T>& last_element = m_Objects[--m_ObjectCount]; // Get last element
	new (&object_to_delete) Pair<ID, T>(std::move(last_element)); // Swap last element and element to delete

	indices[object_to_delete.first].index = in.index; // object to delete here is actually the last element we just swapped
	in.index = INVALID_INDEX; // Invalidate old index
	indices[m_FreelistEnqueue].next_free = id;
	m_FreelistEnqueue = id;
}

template<typename T>
T* PackedArray<T>::Lookup(ID id) const
{
	Index* indices = this->GetIndexArray();
	Index& in = indices[id];

	if (in.index == INVALID_INDEX)
		return NULL;

	return &m_Objects[in.index].second;
}

template<typename T>
FORCEINLINE const typename PackedArray<T>::Iterator PackedArray<T>::begin() const noexcept
{
	return Iterator(m_Objects);
}

template<typename T>
FORCEINLINE const typename PackedArray<T>::Iterator PackedArray<T>::end() const noexcept
{
	return Iterator(m_Objects + m_ObjectCount);
}

template<typename T>
FORCEINLINE typename PackedArray<T>::Iterator PackedArray<T>::begin() noexcept
{
	return Iterator(m_Objects);
}

template<typename T>
FORCEINLINE typename PackedArray<T>::Iterator PackedArray<T>::end() noexcept
{
	return Iterator(m_Objects + m_ObjectCount);
}

template<typename T>
FORCEINLINE typename PackedArray<T>::CIterator PackedArray<T>::cbegin() const noexcept
{
	return CIterator(m_Objects);
}

template<typename T>
FORCEINLINE typename PackedArray<T>::CIterator PackedArray<T>::cend() const noexcept
{
	return CIterator(m_Objects + m_ObjectCount);
}