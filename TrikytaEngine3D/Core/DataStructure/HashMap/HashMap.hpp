#pragma once

#include <Core/Misc/Defines/Common.hpp>
#include <Core/DataStructure/Tuple/Pair.hpp>
#include <Core/DataStructure/Vector/Vector.hpp>

TRE_NS_START

enum CollisionSolvingMethod
{
	PROBING, // Probing
	OPEN_ADR // Open adressing
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

template<typename K, typename V, usize SIZE = 13, CollisionSolvingMethod COLLISION_SOLVING = OPEN_ADR>
class HashMap
{
};

template<typename K, typename V, usize SIZE>
class HashMap<K, V, SIZE, OPEN_ADR>
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

	static const usize DEFAULT_TABLE_CAPACITY = SIZE;
	static const usize DEFAULT_LIST_CAPACITY = 1;

	HashTab_t m_HashTable[DEFAULT_TABLE_CAPACITY];
};

/***********************************************************/
/************************ PROBING **************************/
/***********************************************************/

template<typename K, typename V, usize SIZE>
class HashMap<K, V, SIZE, PROBING>
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
	FORCEINLINE usize CalculateIndex(const K& key) const;
	FORCEINLINE HashTab_t InternalListCheck(usize index);

	static const usize DEFAULT_TABLE_CAPACITY = SIZE;
	static const usize DEFAULT_LIST_CAPACITY = 1;

	HashTab_t m_HashTable;
};

#include "HashMap.inl"

TRE_NS_END
