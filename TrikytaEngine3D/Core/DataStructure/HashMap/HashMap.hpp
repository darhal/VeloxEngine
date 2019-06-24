#pragma once

#include <Core/Misc/Defines/Common.hpp>
#include <Core/DataStructure/Tuple/Pair.hpp>
#include <Core/DataStructure/Vector/Vector.hpp>

TRE_NS_START

enum HashCollisionSolvingMethod
{
	PROBING, // Probing
	CHAINING // Open adressing
};

template<typename T>
usize Hash(const T& val)
{
	return std::hash<T>{}(val);
}

template<>
usize Hash(const int32& val)
{
	if (val == 5 || val == 9) return 3;
	return val;
}

template<typename K, typename V, HashCollisionSolvingMethod COLLISION_SOLVING = CHAINING, usize SIZE = 7>
class HashMap
{
};

template<typename K, typename V, usize SIZE>
class HashMap<K, V, CHAINING, SIZE>
{
public:
	typedef Pair<K, V> HashNode;
	typedef Vector<HashNode> HashTab_t;

	HashMap();

	template<typename... Args>
	FORCEINLINE HashNode& Emplace(const K& key, Args&&... args);

	template<typename... Args>
	FORCEINLINE HashNode& Emplace(K&& key, Args&&... args);

	FORCEINLINE HashNode& Put(const K& key, const V& value);

	FORCEINLINE HashNode& Put(K&& key, V&& value);

	FORCEINLINE V* Get(const K& key);

	FORCEINLINE const V& Get(const K& key) const;

	FORCEINLINE V& operator[](const K& key);

	FORCEINLINE void Remove(const K& key);

	FORCEINLINE bool ContainsKey(const K& key) const;

	FORCEINLINE void Clear();

	FORCEINLINE bool IsEmpty() const;

private:
	FORCEINLINE usize CalculateIndex(const K& key) const;
	FORCEINLINE HashTab_t* InternalListCheck(usize index);

	static const usize DEFAULT_LIST_CAPACITY = 1;

	HashTab_t m_HashTable[SIZE];
};

/***********************************************************/
/************************ PROBING **************************/
/***********************************************************/

usize Probe(usize x, uint32 b = 1, uint32 a = 0) // Linear by default, Quadratic probing if a is not 0
{
	return a * (x * x) + b * x;
}

template<typename K, typename V, usize SIZE>
class HashMap<K, V, PROBING, SIZE>
{
public:
	typedef Pair<K, V> HashNode;
	typedef HashNode* HashTab_t;
	typedef HashMap<K, V, PROBING, SIZE> HT;
	class Iterator;

	HashMap();

	template<typename... Args>
	FORCEINLINE HashNode& Emplace(const K& key, Args&&... args);

	template<typename... Args>
	FORCEINLINE HashNode& Emplace(K&& key, Args&&... args);

	FORCEINLINE HashNode& Put(const K& key, const V& value);

	FORCEINLINE HashNode& Put(K&& key, V&& value);

	FORCEINLINE V* Get(const K& key);

	FORCEINLINE const V& Get(const K& key) const;

	FORCEINLINE V& operator[](const K& key);

	FORCEINLINE void Remove(const K& key);

	FORCEINLINE bool ContainsKey(const K& key) const;

	FORCEINLINE void Clear();

	FORCEINLINE bool IsEmpty() const;

	FORCEINLINE usize GetSize() const;

	FORCEINLINE Iterator begin() noexcept;

	FORCEINLINE Iterator end() noexcept;

private:
	FORCEINLINE usize CalculateHash(const K& key) const;

	FORCEINLINE usize CalculateIndex(const usize hash, const uint32 x) const;

	FORCEINLINE HashTab_t CalculateAdress(const K& key);

	FORCEINLINE HashTab_t InternalListCheck(const K& key);

	FORCEINLINE void Resize(usize newSize);

	FORCEINLINE HashTab_t GetOrEmplace(const K& key);

	FORCEINLINE HashTab_t Reinsert(HashTab_t src, HashTab_t dest);

	static const usize  DEFAULT_LIST_CAPACITY = 7;
	static const double DEFAULT_LOAD_FACTOR;
	static const int8	TOMBSTONE_MARKER	  = -1;

	HashTab_t m_HashTable;
	usize m_Capacity;
	usize m_UsedBuckets;

	// TODO FIX THESE ITERATORS
	class Iterator
	{
	public:
		Iterator(HT* instance) noexcept : m_Instance(instance), m_CurrentNode(m_Instance->m_HashTable) { }
		Iterator(HT* instance, const HashTab_t n) noexcept : m_Instance(instance), m_CurrentNode(n) { }
		bool operator!=(const Iterator& iterator) { return m_CurrentNode != iterator.m_CurrentNode; }
		HashNode& operator*() const { return *m_CurrentNode; }

		Iterator& operator=(HashTab_t n)
		{
			this->m_CurrentNode = n;
			return *this;
		}

		Iterator& operator++()
		{
			usize i = 1;
			HashTab_t adr = m_CurrentNode + i;
			HashTab_t endAdr = m_Instance->m_HashTable + m_Instance->m_Capacity;
			int8 elementMarker = *reinterpret_cast<int8*>(adr);

			while ((elementMarker == NULL || elementMarker == TOMBSTONE_MARKER) && (adr < endAdr)) {
				i++;
				adr = m_CurrentNode + i;
				elementMarker = *reinterpret_cast<int8*>(adr);
			}

			if (adr >= endAdr) {
				m_CurrentNode = NULL;
				return *this;
			}

			m_CurrentNode = adr;
			return *this;
		}

		Iterator operator++(int)
		{
			Iterator iterator = *this;
			++*this;
			return iterator;
		}

		Iterator& operator--()
		{
			ssize i = -1;
			HashTab_t adr = m_CurrentNode + i;
			int8 elementMarker = *reinterpret_cast<int8*>(adr);

			while ((elementMarker == NULL || elementMarker == TOMBSTONE_MARKER) && (adr > m_Instance->m_HashTable)) {
				i--;
				adr = m_CurrentNode + i;
				elementMarker = *reinterpret_cast<int8*>(adr);
			}

			m_CurrentNode = adr;
			return *this;
		}

		Iterator operator--(int)
		{
			Iterator iterator = *this;
			--*this;
			return iterator;
		}

	private:
		HashTab_t m_CurrentNode;
		const HT* m_Instance;
	};
};

template<typename K, typename V, usize S>
const double HashMap<K, V, PROBING, S>::DEFAULT_LOAD_FACTOR = 0.65;

#include "HashMap.inl"

TRE_NS_END
