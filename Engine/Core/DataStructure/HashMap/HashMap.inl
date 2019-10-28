template<typename K, typename V, usize S>
HashMap<K, V, CHAINING, S>::HashMap()
{
}

template<typename K, typename V, usize S>
FORCEINLINE typename HashMap<K, V, CHAINING, S>::HashNode& HashMap<K, V, CHAINING, S>::Put(const K& key, const V& value)
{
	usize index = this->CalculateIndex(key);
	HashTab_t* listAdr = this->InternalListCheck(index); 
	return listAdr->EmplaceBack(key, value);
}

template<typename K, typename V, usize S>
FORCEINLINE typename HashMap<K, V, CHAINING, S>::HashNode& HashMap<K, V, CHAINING, S>::Put(K&& key, V&& value)
{
	usize index = this->CalculateIndex(key);
	HashTab_t* listAdr = this->InternalListCheck(index);
	return listAdr->EmplaceBack(std::forward<K>(key), std::forward<V>(value));
}

template<typename K, typename V, usize S>
template<typename... Args>
FORCEINLINE typename HashMap<K, V, CHAINING, S>::HashNode& HashMap<K, V, CHAINING, S>::Emplace(const K& key, Args&&... args)
{
	usize index = this->CalculateIndex(key);
	HashTab_t* listAdr = this->InternalListCheck(index);
	return listAdr->EmplaceBack(key, std::forward<Args>(args)...);
}

template<typename K, typename V, usize S>
template<typename... Args>
FORCEINLINE typename HashMap<K, V, CHAINING, S>::HashNode& HashMap<K, V, CHAINING, S>::Emplace(K&& key, Args&& ...args)
{
	usize index = this->CalculateIndex(key);
	HashTab_t* listAdr = this->InternalListCheck(index);
	return listAdr->EmplaceBack(std::forward<K>(key), std::forward<Args>(args)...);
}

template<typename K, typename V, usize S>
FORCEINLINE V* HashMap<K, V, CHAINING, S>::Get(const K& key)
{
	usize index = this->CalculateIndex(key);
	for (HashNode& node : m_HashTable[index]) {
		if (node.first == key) {
			return &node.second;
		}
	}
	return NULL;
}

template<typename K, typename V, usize S>
FORCEINLINE const V& HashMap<K, V, CHAINING, S>::Get(const K& key) const
{
	usize index = this->CalculateIndex(key);
	for (const HashNode& node : m_HashTable[index]) {
		if (node.first == key) {
			return node.second;
		}
	}
	this->Put();
}

template<typename K, typename V, usize S>
FORCEINLINE V& HashMap<K, V, CHAINING, S>::operator[](const K& key)
{
	V* res = this->Get(key);
	if (res) {
		return *res;
	}
	HashNode* n = this->Put(key);
	return n->second;
}

template<typename K, typename V, usize S>
FORCEINLINE void HashMap<K, V, CHAINING, S>::Remove(const K& key)
{
	usize index = this->CalculateIndex(key);
	for (typename HashTab_t::Iterator it = m_HashTable.begin(); it != m_HashTable.end(); it++) {
		m_HashTable->Erease(it);
	}
}

template<typename K, typename V, usize S>
FORCEINLINE bool HashMap<K, V, CHAINING, S>::ContainsKey(const K& key) const
{
	usize index = this->CalculateIndex(key);
	HashTab_t* listAdr = m_HashTable + index;
	if (!*((int32*)listAdr)) {
		return false;
	}
	for (const HashNode& node : m_HashTable[index]) {
		if (node.first == key) {
			return true;
		}
	}
	return false;
}

template<typename K, typename V, usize S>
FORCEINLINE void HashMap<K, V, CHAINING, S>::Clear()
{
	for (usize i = 0; i < S; i++) {
		int32* adr = (int32*)(m_HashTable + i);
		if (!*adr) {
			m_HashTable[i].Clear();
		}
	}
}

template<typename K, typename V, usize S>
FORCEINLINE bool HashMap<K, V, CHAINING, S>::IsEmpty() const
{
	for (usize i = 0; i < S; i++) {
		int32* adr = (int32*)(m_HashTable + i);
		if (*adr && !m_HashTable[i].IsEmpty()) {
			return false;
		}
	}
	return true;
}

template<typename K, typename V, usize SIZE>
FORCEINLINE usize HashMap<K, V, PROBING, SIZE>::GetSize() const
{
	return m_UsedBuckets;
}

template<typename K, typename V, usize S>
FORCEINLINE usize HashMap<K, V, CHAINING, S>::CalculateIndex(const K& key) const
{
	usize hash = Hash::GetHash(key);
	return hash % S;
}

template<typename K, typename V, usize S>
FORCEINLINE typename HashMap<K, V, CHAINING, S>::HashTab_t* HashMap<K, V, CHAINING, S>::InternalListCheck(usize index)
{
	/*if (!m_HashTable) {
		m_HashTable = Allocate<HashTab_t>(S);
		for (usize i = 0; i < S; i++) {
			int32* adr = (int32*)(m_HashTable + i);
			*adr = NULL;
		}
	}*/
	HashTab_t* listAdr = m_HashTable + index;
	/*if (!*((int32*)listAdr)) {
		new (listAdr) HashTab_t(DEFAULT_LIST_CAPACITY);
	}*/
	return listAdr;
}

/***********************************************************/
/************************ PROBING **************************/
/***********************************************************/

// TODO : This memory leak must be fixed! dtor is doing nothing apparently.

template<typename K, typename V, usize S>
HashMap<K, V, PROBING, S>::HashMap() : m_HashTable(NULL), m_Capacity(S), m_UsedBuckets(0)
{
}

template<typename K, typename V, usize S>
HashMap<K, V, PROBING, S>::~HashMap()
{
	if (m_HashTable != NULL){
        DestroyMap(*this);
	    ::operator delete(m_HashTable);
		m_HashTable = NULL;
	}
}


template<typename K, typename V, usize S>
FORCEINLINE typename HashMap<K, V, PROBING, S>::HashNode& HashMap<K, V, PROBING, S>::Put(const K& key, const V& value)
{
	HashTab_t listAdr = this->CalculateAdress(key);
	new (listAdr) HashNode(key, value);
	return *listAdr;
}

template<typename K, typename V, usize S>
FORCEINLINE typename HashMap<K, V, PROBING, S>::HashNode& HashMap<K, V, PROBING, S>::Put(K&& key, V&& value)
{
	HashTab_t listAdr = this->CalculateAdress(key);
	new (listAdr) HashNode(std::forward<K>(key), std::forward<V>(value));
	return *listAdr;
}

template<typename K, typename V, usize S>
template<typename... Args>
FORCEINLINE typename HashMap<K, V, PROBING, S>::HashNode& HashMap<K, V, PROBING, S>::Emplace(const K& key, Args&&... args)
{
	HashTab_t listAdr = this->CalculateAdress(key);
	new (listAdr) HashNode(key, std::forward<Args>(args)...);
	return *listAdr;
}

template<typename K, typename V, usize S>
template<typename... Args>
FORCEINLINE typename HashMap<K, V, PROBING, S>::HashNode& HashMap<K, V, PROBING, S>::Emplace(K&& key, Args&& ...args)
{
	HashTab_t listAdr = this->CalculateAdress(key);
	new (listAdr) HashNode(std::forward<K>(key), std::forward<Args>(args)...);
	return *listAdr;
}

template<typename K, typename V, usize S>
FORCEINLINE V* HashMap<K, V, PROBING, S>::Get(const K& key) const
{
	if (m_HashTable == NULL)
		return NULL;

	uint32 x = 0;
	usize hash = Hash::GetHash(key);
	usize initialIndex = this->CalculateIndex(hash, x);
	HashTab_t listAdr = m_HashTable + initialIndex;

	if (!*(reinterpret_cast<int8*>(listAdr))) 
		return NULL;

	HashTab_t startAdr = listAdr;
	HashTab_t tombstoneAdr = NULL;
	int8 elementMarker = *reinterpret_cast<int8*>(listAdr);

	// Probing solution since there is a collision here
	while (elementMarker && (elementMarker == TOMBSTONE_MARKER || !(listAdr->first == key))) {
		listAdr = m_HashTable + this->CalculateIndex(hash, ++x);
		elementMarker = *reinterpret_cast<int8*>(listAdr);

		// If we hit TOMBSTONE then later swap between the tombstone and the value
		if (elementMarker == TOMBSTONE_MARKER && !tombstoneAdr) { // Hit Tombstone (previously deleted)
			tombstoneAdr = listAdr;
		}

		if (listAdr == startAdr) {// we finished a cycle and the element doesn't exist.
			return NULL;
		}
	}
	
	if (elementMarker && elementMarker != TOMBSTONE_MARKER && listAdr->first == key) {

		if (tombstoneAdr) {	// Seen tombstone already
			CopyRangeTo(listAdr, tombstoneAdr, 1); // perform a copy
			*reinterpret_cast<int8*>(listAdr) = TOMBSTONE_MARKER;
			listAdr = tombstoneAdr; // listAdr is the tombstone adress that we will return
		}

		return &(listAdr->second);
	}

	return NULL;
}

template<typename K, typename V, usize S>
FORCEINLINE const V& HashMap<K, V, PROBING, S>::Get(const K& key)
{
	HashTab_t listAdr = this->GetOrEmplace(key);

	while (!listAdr) {
		this->Resize(m_Capacity + S * 2);
		listAdr = this->GetOrEmplace(key);		// Should handle probing
	}

	return listAdr->second;
}

template<typename K, typename V, usize S>
FORCEINLINE const typename HashMap<K, V, PROBING, S>::HashNode& HashMap<K, V, PROBING, S>::GetPair(const K& key) const
{
	HashTab_t listAdr = this->GetOrEmplace(key);

	while (!listAdr) {
		this->Resize(m_Capacity + S * 2);
		listAdr = this->GetOrEmplace(key);		// Should handle probing
	}

	return *listAdr;
}

template<typename K, typename V, usize S>
FORCEINLINE typename HashMap<K, V, PROBING, S>::HashNode* HashMap<K, V, PROBING, S>::GetPairPtr(const K& key) const
{
	if (m_HashTable == NULL)
		return NULL;

	uint32 x = 0;
	usize hash = Hash::GetHash(key);
	usize initialIndex = this->CalculateIndex(hash, x);
	HashTab_t listAdr = m_HashTable + initialIndex;

	if (!*(reinterpret_cast<int8*>(listAdr))) 
		return NULL;

	HashTab_t startAdr = listAdr;
	HashTab_t tombstoneAdr = NULL;
	int8 elementMarker = *reinterpret_cast<int8*>(listAdr);

	// Probing solution since there is a collision here
	while (elementMarker && (elementMarker == TOMBSTONE_MARKER || !(listAdr->first == key))) {
		listAdr = m_HashTable + this->CalculateIndex(hash, ++x);
		elementMarker = *reinterpret_cast<int8*>(listAdr);

		// If we hit TOMBSTONE then later swap between the tombstone and the value
		if (elementMarker == TOMBSTONE_MARKER && !tombstoneAdr) { // Hit Tombstone (previously deleted)
			tombstoneAdr = listAdr;
		}

		if (listAdr == startAdr) {// we finished a cycle and the element doesn't exist.
			return NULL;
		}
	}
	
	if (elementMarker && elementMarker != TOMBSTONE_MARKER && listAdr->first == key) {

		if (tombstoneAdr) {	// Seen tombstone already
			CopyRangeTo(listAdr, tombstoneAdr, 1); // perform a copy
			*reinterpret_cast<int8*>(listAdr) = TOMBSTONE_MARKER;
			listAdr = tombstoneAdr; // listAdr is the tombstone adress that we will return
		}

		return listAdr;
	}

	return NULL;
}

template<typename K, typename V, usize S>
FORCEINLINE V& HashMap<K, V, PROBING, S>::operator[](const K& key)
{
	HashTab_t listAdr = this->GetOrEmplace(key);

	while (!listAdr) {
		this->Resize(m_Capacity + S * 2);
		listAdr = this->GetOrEmplace(key);		// Should handle probing
	}

	return listAdr->second;
}

template<typename K, typename V, usize S>
FORCEINLINE V& HashMap<K, V, PROBING, S>::operator[](const K& key) const
{
	V* ptr = this->Get(key);
	ASSERTF(ptr == NULL, "Attempt to access element at an invalid key.");
	return *ptr;
}

template<typename K, typename V, usize S>
FORCEINLINE void HashMap<K, V, PROBING, S>::Remove(const K& key)
{
	uint32 x = 0;
	usize hash = Hash::GetHash(key);
	usize initialIndex = this->CalculateIndex(hash, x);
	HashTab_t listAdr = m_HashTable + initialIndex;

	if (!*(reinterpret_cast<int8*>(listAdr)))
		return;

	HashTab_t startAdr = listAdr;
	int8 elementMarker = *reinterpret_cast<int8*>(listAdr);

	// Probing solution since there is a collision here
	while (elementMarker && (elementMarker == TOMBSTONE_MARKER || !(listAdr->first == key))) {
		listAdr = m_HashTable + this->CalculateIndex(hash, ++x);
		elementMarker = *reinterpret_cast<int8*>(listAdr);

		if (listAdr == startAdr) // we finished a cycle and the element doesn't exist.
			return; 
	}

	if (elementMarker) { // We found the element
		m_UsedBuckets--;
		Destroy(&listAdr->key); // listAdr->~HashNode();
		Destroy(&listAdr->value);
		*reinterpret_cast<int8*>(listAdr) = TOMBSTONE_MARKER; // Put Tombstone as its deleted
	}
}

template<typename K, typename V, usize S>
FORCEINLINE bool HashMap<K, V, PROBING, S>::ContainsKey(const K& key) const
{
	return !(this->Get(key));
}

template<typename K, typename V, usize S>
FORCEINLINE void HashMap<K, V, PROBING, S>::Clear()
{
	for (usize i = 0; i < m_Capacity; i++) {
		HashTab_t listAdr = m_HashTable + i;
		int8 elementMarker = *reinterpret_cast<int8*>(listAdr);

		if (elementMarker && elementMarker != TOMBSTONE_MARKER) {
			Destroy(&listAdr->key); // listAdr->~HashNode();
			Destroy(&listAdr->value);
		}

		*reinterpret_cast<int8*>(listAdr) = int8(NULL);
	}
	
	m_UsedBuckets = 0;
}

template<typename K, typename V, usize S>
FORCEINLINE bool HashMap<K, V, PROBING, S>::IsEmpty() const
{
	return m_UsedBuckets == 0;
}

template<typename K, typename V, usize S>
FORCEINLINE void HashMap<K, V, PROBING, S>::Resize(usize newSize)
{
	if (newSize < m_Capacity) 
		return;

	m_UsedBuckets = 0;
	HashTab_t newAdr = Allocate<HashNode>(newSize);

	for (usize i = 0; i < newSize; i++) {
		*reinterpret_cast<int8*>(newAdr + i) = int8(NULL);
	}

	usize oldCapacity = m_Capacity;
	m_Capacity = newSize;

	for (usize i = 0; i < oldCapacity; i++) {
		HashTab_t srcAdr = m_HashTable + i;
		if (*(reinterpret_cast<int8*>(srcAdr))) {
			this->Reinsert(srcAdr, newAdr);
		}
	}

	Free(m_HashTable);
	m_HashTable = newAdr;
}

template<typename K, typename V, usize S>
FORCEINLINE typename HashMap<K, V, PROBING, S>::HashTab_t HashMap<K, V, PROBING, S>::CalculateAdress(const K& key)
{
	//if (m_UsedBuckets >= (usize) (m_Capacity * DEFAULT_LOAD_FACTOR))
	//	this->Resize(m_Capacity + S * 2);

	HashTab_t listAdr = this->InternalListCheck(key); // Should handle probing

	// Resize and insert in the right place while there is no place left.
	while (!listAdr) {
		this->Resize(m_Capacity + S * 2);
		listAdr = this->InternalListCheck(key);		// Should handle probing
	}

	return listAdr;
}

template<typename K, typename V, usize S>
FORCEINLINE typename HashMap<K, V, PROBING, S>::HashTab_t HashMap<K, V, PROBING, S>::Reinsert(HashTab_t src, HashTab_t dest)
{
	uint32 x = 0;
	K& key = (src->first);
	usize hash = Hash::GetHash(key);
	usize initialIndex = this->CalculateIndex(hash, x);
	HashTab_t listAdr = dest + initialIndex;
	HashTab_t startAdr = listAdr;
	int8 elementMarker = *reinterpret_cast<int8*>(listAdr);

	// Probing solution since there is a collision here
	while (elementMarker && !(listAdr->first == key)) {
		listAdr = dest + this->CalculateIndex(hash, ++x);
		elementMarker = *reinterpret_cast<int8*>(listAdr);

		if (listAdr == startAdr) // There is no slots (we finished a cycle) we are obliged to return NULL pointer.
			return NULL;
	}

	m_UsedBuckets++;
	CopyRangeTo(src, listAdr, 1);
	return listAdr;
}

template<typename K, typename V, usize S>
FORCEINLINE typename HashMap<K, V, PROBING, S>::HashTab_t HashMap<K, V, PROBING, S>::InternalListCheck(const K& key)
{
	uint32 x = 0;
	usize hash = Hash::GetHash(key);
	usize initialIndex = this->CalculateIndex(hash, x);

	if (!m_HashTable) {
		m_HashTable = Allocate<HashNode>(m_Capacity);

		for (usize i = 0; i < m_Capacity; i++) {
			int8* adr = reinterpret_cast<int8*>(m_HashTable + i);
			*adr = int8(NULL);
		}

		m_UsedBuckets++;
		HashTab_t listAdr = m_HashTable + initialIndex; // Its safe since the table is empty
		return listAdr;
	}

	HashTab_t listAdr = m_HashTable + initialIndex;
	HashTab_t startAdr = listAdr;
	HashTab_t tombstoneAdr = NULL;
	int8 elementMarker = *reinterpret_cast<int8*>(listAdr);

	// Probing solution since there is a collision here
	while (elementMarker && (elementMarker == TOMBSTONE_MARKER || !(listAdr->first == key))) {
		listAdr = m_HashTable + this->CalculateIndex(hash, ++x);
		elementMarker = *reinterpret_cast<int8*>(listAdr);

		// If we hit TOMBSTONE then later swap between the tombstone and the value
		if (elementMarker == TOMBSTONE_MARKER && !tombstoneAdr) { // Hit Tombstone (previously deleted)
			tombstoneAdr = listAdr;
		}

		if (listAdr == startAdr) // There is no slots (we finished a cycle) we are obliged to return NULL pointer.
			return NULL; 
	}

	bool seenTombstone = (tombstoneAdr);

	if (elementMarker) { // Means directly they have same key!
		//if (listAdr->first == key) { // double check might be completely unnecessary 
		// Call dtor since the same key is here already
		Destroy(&listAdr->key); // listAdr->~HashNode();
		Destroy(&listAdr->value);

		if (seenTombstone) {
			*reinterpret_cast<int8*>(listAdr) = TOMBSTONE_MARKER;
			return tombstoneAdr;	// listAdr is the tombstone adress that we will return
		}
		//}
	}else if (seenTombstone) {
		return tombstoneAdr; // listAdr is the tombstone adress that we will return
	}

	m_UsedBuckets++;
	return listAdr;
}

template<typename K, typename V, usize S>
FORCEINLINE typename HashMap<K, V, PROBING, S>::HashTab_t HashMap<K, V, PROBING, S>::GetOrEmplace(const K& key)
{
	uint32 x = 0;
	usize hash = Hash::GetHash(key);
	usize initialIndex = this->CalculateIndex(hash, x);

	if (!m_HashTable) {
		m_HashTable = Allocate<HashNode>(m_Capacity);

		for (usize i = 0; i < m_Capacity; i++) {
			int8* adr = reinterpret_cast<int8*>(m_HashTable + i);
			*adr = int8(NULL);
		}

		m_UsedBuckets++;
		HashTab_t listAdr = m_HashTable + initialIndex; // Its safe since the table is empty

		new (listAdr) HashNode(key, V());
		return listAdr;
	}

	HashTab_t listAdr = m_HashTable + initialIndex;
	HashTab_t startAdr = listAdr;
	HashTab_t tombstoneAdr = NULL;
	int8 elementMarker = *reinterpret_cast<int8*>(listAdr);

	// Probing solution since there is a collision here
	while (elementMarker && (elementMarker == TOMBSTONE_MARKER || !(listAdr->first == key))) {
		listAdr = m_HashTable + this->CalculateIndex(hash, ++x);
		elementMarker = *reinterpret_cast<int8*>(listAdr);

		// If we hit TOMBSTONE then later swap between the tombstone and the value
		if (elementMarker == TOMBSTONE_MARKER && tombstoneAdr == NULL) { // Hit Tombstone (previously deleted)
			tombstoneAdr = listAdr;
		}

		if (listAdr == startAdr) // There is no slots (we finished a cycle) we are obliged to return NULL pointer.
			return NULL;
	}


	if (elementMarker && elementMarker != TOMBSTONE_MARKER && listAdr->first == key) {
		if (tombstoneAdr) {	// Seen tombstone already
			CopyRangeTo(listAdr, tombstoneAdr, 1); // perform a copy
			*reinterpret_cast<int8*>(listAdr) = TOMBSTONE_MARKER;
			listAdr = tombstoneAdr; // listAdr is the tombstone adress that we will return
		}

		return listAdr;
	}else if (tombstoneAdr) {
		new (tombstoneAdr) HashNode(key, V());
		return tombstoneAdr; // listAdr is the tombstone adress that we will return
	}

	m_UsedBuckets++;
	new (listAdr) HashNode(key, V());
	return listAdr; // listAdr is the tombstone adress that we will return
}

template<typename K, typename V, usize S>
FORCEINLINE usize HashMap<K, V, PROBING, S>::CalculateIndex(const usize hash, const uint32 x) const
{
	return ((hash + Probe(x)) % m_Capacity);
}

template<typename K, typename V, usize S>
FORCEINLINE usize HashMap<K, V, PROBING, S>::CalculateHash(const K& key) const
{
	return Hash::GetHash(key);
}

template<typename K, typename V, usize S>
template<typename Ki, typename Vi, typename std::enable_if<HAVE_DTOR(Ki) || HAVE_DTOR(Vi), int>::type>
FORCEINLINE void HashMap<K, V, PROBING, S>::DestroyMap(HashMap<Ki, Vi, PROBING, S>& map)
{
    for (usize i = 0; i < map.m_Capacity; i++) {
		HashTab_t listAdr = map.m_HashTable + i;
		int8 elementMarker = *reinterpret_cast<int8*>(listAdr);

		if (elementMarker && elementMarker != TOMBSTONE_MARKER) {
			Destroy(&listAdr->key); // listAdr->~HashNode();
			Destroy(&listAdr->value);
		}
	}
}

template<typename K, typename V, usize S>
template<typename Ki, typename Vi, typename std::enable_if<NO_DTOR(Ki) && NO_DTOR(Vi), int>::type>
FORCEINLINE void HashMap<K, V, PROBING, S>::DestroyMap(HashMap<Ki, Vi, PROBING, S>& map)
{
    // DO NOTHING
}

////////////////////////////////////// ITERATORS //////////////////////////////////////

//TODO : This should be fixed since there is holes in the array ( I think its done)
template<typename K, typename V, usize S>
FORCEINLINE typename HashMap<K, V, PROBING, S>::Iterator HashMap<K, V, PROBING, S>::begin() noexcept
{
	if (m_UsedBuckets == 0)
		return Iterator(this, m_HashTable);

	usize i = 0;
	HashTab_t adr = m_HashTable + i;
	int8 elementMarker = *reinterpret_cast<int8*>(adr);

	while ((!elementMarker || elementMarker == TOMBSTONE_MARKER) && i < m_Capacity) {
		i++;
		adr = m_HashTable + i;
		elementMarker = *reinterpret_cast<int8*>(adr);
	}

	return Iterator(this, adr);
}

template<typename K, typename V, usize S>
FORCEINLINE typename HashMap<K, V, PROBING, S>::Iterator HashMap<K, V, PROBING, S>::end() noexcept
{
	if (m_UsedBuckets == 0)
		return Iterator(this, m_HashTable);

	/*ssize i = m_Capacity - 1;
	HashTab_t adr = m_HashTable + i;
	int8 elementMarker = *reinterpret_cast<int8*>(adr);

	while ((!elementMarker || elementMarker == TOMBSTONE_MARKER) && (i > 0)) {
		i--;
		adr = m_HashTable + i;
		elementMarker = *reinterpret_cast<int8*>(adr);
	}*/

	return Iterator(this, NULL);
}

template<typename K, typename V, usize S>
FORCEINLINE const typename HashMap<K, V, PROBING, S>::Iterator HashMap<K, V, PROBING, S>::begin() const noexcept
{
	if (m_UsedBuckets == 0)
		return Iterator(this, m_HashTable);

	usize i = 0;
	HashTab_t adr = m_HashTable + i;
	int8 elementMarker = *reinterpret_cast<int8*>(adr);

	while ((!elementMarker || elementMarker == TOMBSTONE_MARKER) && i < m_Capacity) {
		i++;
		adr = m_HashTable + i;
		elementMarker = *reinterpret_cast<int8*>(adr);
	}

	return Iterator(this, adr);
}

template<typename K, typename V, usize S>
FORCEINLINE const typename HashMap<K, V, PROBING, S>::Iterator HashMap<K, V, PROBING, S>::end() const noexcept
{
	if (m_UsedBuckets == 0)
		return Iterator(this, m_HashTable);

	/*ssize i = m_Capacity - 1;
	HashTab_t adr = m_HashTable + i;
	int8 elementMarker = *reinterpret_cast<int8*>(adr);

	while ((!elementMarker || elementMarker == TOMBSTONE_MARKER) && (i > 0)) {
		i--;
		adr = m_HashTable + i;
		elementMarker = *reinterpret_cast<int8*>(adr);
	}*/

	return Iterator(this, NULL);
}

template<typename K, typename V, usize S>
FORCEINLINE typename HashMap<K, V, PROBING, S>::CIterator HashMap<K, V, PROBING, S>::cbegin() const noexcept
{
	if (m_UsedBuckets == 0)
		return CIterator(this, m_HashTable);

	usize i = 0;
	HashTab_t adr = m_HashTable + i;
	int8 elementMarker = *reinterpret_cast<int8*>(adr);

	while ((!elementMarker || elementMarker == TOMBSTONE_MARKER) && i < m_Capacity) {
		i++;
		adr = m_HashTable + i;
		elementMarker = *reinterpret_cast<int8*>(adr);
	}

	return CIterator(this, adr);
}

template<typename K, typename V, usize S>
FORCEINLINE typename HashMap<K, V, PROBING, S>::CIterator HashMap<K, V, PROBING, S>::cend() const noexcept
{
	if (m_UsedBuckets == 0)
		return CIterator(this, m_HashTable);

	/*ssize i = m_Capacity - 1;
	HashTab_t adr = m_HashTable + i;
	int8 elementMarker = *reinterpret_cast<int8*>(adr);

	while ((!elementMarker || elementMarker == TOMBSTONE_MARKER) && (i > 0)) {
		i--;
		adr = m_HashTable + i;
		elementMarker = *reinterpret_cast<int8*>(adr);
	}*/

	return CIterator(this, NULL);
}