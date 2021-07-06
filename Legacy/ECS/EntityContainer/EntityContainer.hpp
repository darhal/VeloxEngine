#pragma once

#include <Legacy/Misc/Defines/Common.hpp>
#include <Legacy/Misc/Defines/Debug.hpp>

TRE_NS_START

class Entity;
class EntityManager;

class EntityContainer
{
public:
	typedef uint32 ID;

	struct Index
	{
		ID index, next_free;
	};

	CONSTEXPR static ID DEFAULT_CAPACITY = 1 << 16;
	CONSTEXPR static ID INVALID_INDEX = ID(-1);
public:
	EntityContainer(EntityManager* manager, uint32 capacity = DEFAULT_CAPACITY);

	Entity& CreateEntity();

	void Remove(ID id);

	Entity* Lookup(ID id) const;

	Entity& Get(ID id) const;

	FORCEINLINE Entity& operator[](ID id) const;

	FORCEINLINE uint32 Size() const { return m_ObjectCount; }

	FORCEINLINE uint32 Capacity() const { return m_Capacity; }
private:
	EntityManager* m_Manager;
	Entity* m_Entities;
	uint32 m_Capacity;
	uint32 m_ObjectCount;
	uint32 m_FreelistEnqueue;
	uint32 m_FreelistDequeue;

	Index* GetIndexArray() const;

	void Reallocate(uint32 new_capacity);
};

FORCEINLINE Entity& EntityContainer::operator[](ID id) const
{
	return this->Get(id);
}

TRE_NS_END