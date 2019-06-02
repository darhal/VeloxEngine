#pragma once

#include <Core/Misc/Defines/Common.hpp>
#include <Core/Misc/Defines/Debug.hpp>
#include <Core/Memory/SmartPtr/UniquePointer.hpp>
#include <utility>

TRE_NS_START

class PoolArena
{
private:
	struct PoolItem
	{
		PoolItem* m_NextItem;

		FORCEINLINE PoolItem* GetNext() { return m_NextItem; };

		FORCEINLINE void SetNext(PoolItem* item_ptr) { m_NextItem = item_ptr; };

		static PoolItem* StorageToItem(void* t) {
			PoolItem* current_item = reinterpret_cast<PoolItem*>(t);
			return current_item;
		}
	};
	PoolItem* m_Items;
	UniquePointer<PoolArena> m_Next;

	friend class MultiPoolAllocator;
public:
	template<typename T>
	PoolArena(usize chunk_num) : m_Next(NULL)
	{
		usize chunk_size = sizeof(T);
		ASSERTF(!(chunk_size < sizeof(PoolItem)), "Given size (%d bytes) is smaller than the Pool Chunk(Item) size(%d).", chunk_size, sizeof(PoolItem));
		m_Items = (PoolItem*) operator new (chunk_size * chunk_num);
		this->Reset(chunk_size, chunk_num);
	}

	PoolArena(usize chunk_size, usize chunk_num) : m_Next(NULL)
	{
		ASSERTF(!(chunk_size < sizeof(PoolItem)), "Given size (%d bytes) is smaller than the Pool Chunk(Item) size(%d).", chunk_size, sizeof(PoolItem));
		m_Items = (PoolItem*) operator new (chunk_size * chunk_num);
		this->Reset(chunk_size, chunk_num);
	}

	void Reset(usize chunk_size, usize chunk_num)
	{
		for (usize i = 1; i < chunk_num; i++) {
			ssize address = (ssize) m_Items + i * chunk_size;
			m_Items[i - 1].SetNext((PoolItem*)address);
		}
		m_Items[chunk_num - 1].SetNext(NULL);
	}

	void SetNext(UniquePointer<PoolArena>&& next) 
	{
		m_Next = next;
	}

	~PoolArena() { 
		printf("DELETE AN ARENA adr = %d\n", m_Items);
		delete m_Items; 
	}

	PoolItem* GetStorage() { return m_Items; };
};

class MultiPoolAllocator
{
public:

	MultiPoolAllocator(usize chunk_size, usize chunk_num) : 
		m_Arena(new PoolArena(chunk_size, chunk_num)), 
		m_Freelist(m_Arena->GetStorage()),
		m_ItemSize(chunk_size), m_ItemsNumber(chunk_num)
	{
		printf("List is empty allocating more Start at = %d\n", m_Arena->GetStorage());
	}

	template<typename T>
	MultiPoolAllocator(usize chunk_num) : 
		m_Arena(new PoolArena<T>(chunk_num)),
		m_Freelist(m_Arena->GetStorage()),
		m_ItemSize(sizeof(T)), m_ItemsNumber(chunk_num)
	{
		printf("List is empty allocating more Start at = %d\n", m_Arena->GetStorage());
	}

	void* Allocate(usize sz = 0)
	{
		if (m_Freelist == NULL) {
			UniquePointer<PoolArena> newArena(new PoolArena(m_ItemSize, m_ItemsNumber));
			printf("List is empty allocating more Start at = %d\n", newArena->GetStorage());
			newArena->SetNext(std::move(m_Arena));
			m_Arena = newArena;
			m_Freelist = m_Arena->GetStorage();
		}
		typename PoolArena::PoolItem* item = m_Freelist;
		m_Freelist = item->GetNext();
		printf("Giving adress = %d\n", item);
		void* result = (void*) item;
		return result;
	}

	template<typename T>
	T* Allocate()
	{
		T* result = (T*) this->Allocate();
		return result;
	}

	template<typename T, typename... Args>
	T* Allocate(Args&&... args)
	{
		T* result = (T*)this->Allocate();
		new (result) T(std::forward<Args>(args)...);
		return result;
	}

	void Deallocate(void* ptr)
	{
		typename PoolArena::PoolItem* current_item = PoolArena::PoolItem::StorageToItem(ptr);
		current_item->SetNext(m_Freelist);
		m_Freelist = current_item;
	}

	//TODO: This is wasting memeory even tho there is space ! (fix this)
	template<typename T>
	void Deallocate(T* ptr)
	{
		ptr->T::~T();
		typename PoolArena::PoolItem* current_item = PoolArena::PoolItem::StorageToItem(ptr);
		current_item->SetNext(m_Freelist);
		m_Freelist = current_item;
	}

private:
	usize m_ItemSize, m_ItemsNumber;
	UniquePointer<PoolArena> m_Arena;
	typename PoolArena::PoolItem* m_Freelist;
};

TRE_NS_END