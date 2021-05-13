#pragma once

#include <iterator>
#include "HashFunction.hpp"
#include <Core/Misc/Defines/Common.hpp>
#include <Core/Memory/Utils/Utils.hpp>
#include <Core/DataStructure/Tuple/Pair.hpp>
#include <Core/DataStructure/Vector/Vector.hpp>


TRE_NS_START

enum HashCollisionSolvingMethod
{
	PROBING, // Probing
	CHAINING // Open adressing
};

template<typename K, typename V, HashCollisionSolvingMethod COLLISION_SOLVING = PROBING, usize SIZE = 19>
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

FORCEINLINE usize Probe(usize x, uint32 b = 1, uint32 a = 0) // Linear by default, Quadratic probing if a is not 0
{
	return a * (x * x) + b * x;
}

template<typename K, typename V, usize SIZE>
class HashMap<K, V, PROBING, SIZE>
{
public:
	typedef Pair<K, V> HashPair;

	struct HashNode {
		enum NodeTag { FREE = 0, OCCUPIED = 1, TOMBSTONE = 2 };

		HashNode(HashNode&& other) : pair(std::move(other.pair)), tag(other.tag)
		{}

		HashPair pair;
		uint8 tag;
	};

	typedef HashNode* HashTab_t;
	typedef HashMap<K, V, PROBING, SIZE> HT;

	template<typename PointerType>
	class GIterator;

	typedef GIterator<HashTab_t> Iterator;
	typedef GIterator<const HashTab_t> CIterator;
public:

	HashMap();

	~HashMap();

	template<typename... Args>
	FORCEINLINE HashPair& Emplace(const K& key, Args&&... args);

	template<typename... Args>
	FORCEINLINE HashPair& Emplace(K&& key, Args&&... args);

	FORCEINLINE HashPair& Put(const K& key, const V& value);

	FORCEINLINE HashPair& Put(K&& key, V&& value);

	FORCEINLINE V* GetKeyPtr(const K& key) const;

	FORCEINLINE const V& Get(const K& key);

	FORCEINLINE const HashPair& GetPair(const K& key) const;

	FORCEINLINE HashPair* GetPairPtr(const K& key) const;

	FORCEINLINE V& operator[](const K& key);

	FORCEINLINE V& operator[](const K& key) const;

	FORCEINLINE void Remove(const K& key);

	FORCEINLINE bool ContainsKey(const K& key) const;

	FORCEINLINE void Clear();

	FORCEINLINE bool IsEmpty() const;

	FORCEINLINE usize GetSize() const;

	FORCEINLINE Iterator begin() noexcept;
	FORCEINLINE Iterator end() noexcept;

	FORCEINLINE const Iterator begin() const noexcept;
	FORCEINLINE const Iterator end() const noexcept;

	FORCEINLINE CIterator cbegin() const noexcept;
	FORCEINLINE CIterator cend() const noexcept;

private:
	FORCEINLINE usize CalculateHash(const K& key) const;

	FORCEINLINE usize CalculateIndex(const usize hash, const uint32 x) const;

	FORCEINLINE HashTab_t CalculateAdress(const K& key);

	FORCEINLINE HashTab_t InternalListCheck(const K& key);

	FORCEINLINE void Resize(usize newSize);

	FORCEINLINE HashTab_t GetOrEmplace(const K& key);

	FORCEINLINE HashTab_t Reinsert(HashTab_t src, HashTab_t dest);

	template<typename Ki, typename Vi, typename std::enable_if<HAVE_DTOR(Ki) || HAVE_DTOR(Vi), int>::type = 0>
	FORCEINLINE static void DestroyMap(HashMap<Ki, Vi, PROBING, SIZE>& map);

	template<typename Ki, typename Vi, typename std::enable_if<NO_DTOR(Ki) && NO_DTOR(Vi), int>::type = 0>
	FORCEINLINE static void DestroyMap(HashMap<Ki, Vi, PROBING, SIZE>& map);

	static const usize  DEFAULT_LIST_CAPACITY = SIZE;
	static const double DEFAULT_LOAD_FACTOR;
	static const uint8	TOMBSTONE_MARKER	  = HashNode::TOMBSTONE;

	HashTab_t m_HashTable;
	usize m_Capacity;
	usize m_UsedBuckets;

public:
	// TODO FIX THESE ITERATORS (I think its done!)
	
	template<typename DataType>
	class GIterator
	{
	public:
		GIterator(const HashMap<K, V, PROBING, SIZE>* instance) noexcept : m_CurrentNode(m_Instance->m_HashTable), m_Instance(instance) { }
		GIterator(const HashMap<K, V, PROBING, SIZE>* instance, DataType n) noexcept : m_CurrentNode(n), m_Instance(instance) { }
		bool operator!=(const GIterator<DataType>& iterator) { return m_CurrentNode != iterator.m_CurrentNode; }
		HashPair& operator*() const { return m_CurrentNode->pair; }

		GIterator<DataType>& operator=(DataType n)
		{
			this->m_CurrentNode = n;
			return *this;
		}

		GIterator<DataType>& operator++()
		{
			usize i = 1;
			DataType adr = m_CurrentNode + i;
			DataType endAdr = m_Instance->m_HashTable + m_Instance->m_Capacity;
			uint8 elementMarker = adr->tag;

			while (!(elementMarker == HashNode::OCCUPIED) && (adr < endAdr)) {
				adr = m_CurrentNode + ++i;
				elementMarker = adr->tag;
			}

			if (adr >= endAdr) {
				m_CurrentNode = NULL;
				return *this;
			}

			m_CurrentNode = adr;
			return *this;
		}

		GIterator<DataType> operator++(int)
		{
			GIterator<DataType> iterator = *this;
			++*this;
			return iterator;
		}

		GIterator<DataType>& operator--()
		{
			ssize i = -1;
			DataType adr = m_CurrentNode + i;
			uint8 elementMarker = adr->tag;

			while (!(elementMarker == HashNode::OCCUPIED) && (adr > m_Instance->m_HashTable)) {
				adr = m_CurrentNode + --i;
				elementMarker = adr->tag;
			}

			m_CurrentNode = adr;
			return *this;
		}

		GIterator<DataType> operator--(int)
		{
			GIterator<DataType> iterator = *this;
			--*this;
			return iterator;
		}

	private:
		DataType m_CurrentNode;
		const HashMap<K, V, PROBING, SIZE>* m_Instance;
	};
};

template<typename K, typename V, usize S>
const double HashMap<K, V, PROBING, S>::DEFAULT_LOAD_FACTOR = 0.65;

TRE_NS_END

#include "HashMap.inl"