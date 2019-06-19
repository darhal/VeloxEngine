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
	FORCEINLINE usize CalculateHash(const K& key) const;

	FORCEINLINE usize CalculateIndex(const usize hash, const uint32 x) const;

	FORCEINLINE HashTab_t CalculateAdress(const K& key);

	FORCEINLINE HashTab_t InternalListCheck(const K& key);

	FORCEINLINE void Resize(usize newSize);

	static const usize DEFAULT_LIST_CAPACITY = 1;

	HashTab_t m_HashTable;
	usize m_Capacity;
};

#include "HashMap.inl"

TRE_NS_END
