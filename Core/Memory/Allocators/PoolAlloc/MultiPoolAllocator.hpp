#pragma once

#include <Core/Misc/Defines/Common.hpp>
#include <Core/Misc/Defines/Debug.hpp>
#include <Core/Memory/SmartPtr/UniquePointer.hpp>
#include <Core/Memory/Allocators/BaseAlloc/BaseAllocator.hpp>
#include <Core/Memory/Utils/Utils.hpp>
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
	static PoolArena* CreatePoolArena(uint32 chunk_num)
	{
		return new PoolArena(sizeof(T), chunk_num);
	}

	template<typename T>
	explicit PoolArena(uint32 chunk_num) : m_Next(NULL)
	{
		uint32 chunk_size = sizeof(T);
		ASSERTF((chunk_size < sizeof(PoolItem)), "Given size (%u bytes) is smaller than the Pool Chunk(Item) size(%" SZu ").", chunk_size, sizeof(PoolItem));
		m_Items = (PoolItem*) operator new (chunk_size * chunk_num);
		this->Reset(chunk_size, chunk_num);
	}

	PoolArena(uint32 chunk_size, uint32 chunk_num) : m_Next(NULL)
	{
		ASSERTF((chunk_size < sizeof(PoolItem)), "Given size (%u bytes) is smaller than the Pool Chunk(Item) size(%" SZu ").", chunk_size, sizeof(PoolItem));
		m_Items = (PoolItem*) operator new (chunk_size * chunk_num);
		this->Reset(chunk_size, chunk_num);
	}

	void Reset(uint32 chunk_size, uint32 chunk_num)
	{
		for (uint32 i = 1; i < chunk_num; i++) {
			ssize address = (ssize) m_Items + i * chunk_size;
			((PoolItem*)(address - chunk_size))->SetNext((PoolItem*)address);
		}
		ssize address = (ssize)m_Items + (chunk_num - 1) * chunk_size;
		((PoolItem*)address)->SetNext(NULL);
	}

	void SetNext(UniquePointer<PoolArena>&& next) 
	{
		m_Next = next;
	}

	~PoolArena() { 
		delete m_Items; 
	}

	PoolItem* GetStorage() { return m_Items; };
};

class MultiPoolAllocator : BaseAllocator
{
public:

	MultiPoolAllocator(uint32 chunk_size, uint32 chunk_num, bool autoInit = false) : 
		m_ItemSize(chunk_size), m_ItemsNumber(chunk_num),
		//m_Arena(new PoolArena(chunk_size, chunk_num)), 
		m_Freelist(NULL)
		//m_Freelist(m_Arena->GetStorage())
	{
		if (autoInit) {
			m_Arena = new PoolArena(chunk_size, chunk_num);
			m_Freelist = (m_Arena->GetStorage());
		}
	}

	template<typename T>
	MultiPoolAllocator(uint32 chunk_num) : 
		m_Arena(PoolArena::CreatePoolArena<T>(chunk_num)),
		// m_Arena(new PoolArena<T>(chunk_num)),
		m_Freelist(m_Arena->GetStorage()),
		m_ItemSize(sizeof(T)), m_ItemsNumber(chunk_num)
	{}

	MultiPoolAllocator(MultiPoolAllocator&& other) :
		m_ItemSize(other.m_ItemSize), m_ItemsNumber(other.m_ItemsNumber),
		m_Arena(::std::move(other.m_Arena)), m_Freelist(other.m_Freelist)
	{}

	MultiPoolAllocator(MultiPoolAllocator& other) :
		m_ItemSize(other.m_ItemSize), m_ItemsNumber(other.m_ItemsNumber),
		m_Arena(other.m_Arena), m_Freelist(other.m_Freelist)
	{}

	MultiPoolAllocator& operator=(MultiPoolAllocator&& other)
	{
		m_ItemSize = ::std::move(other.m_ItemSize);
		m_ItemsNumber = ::std::move(other.m_ItemsNumber);
		m_Arena = ::std::move(other.m_Arena);
		m_Freelist = ::std::move(other.m_Freelist);

		return *this;
	}

	void* Allocate(uint32 sz = 0, uint32 alignement = 0)
	{
		if (m_Freelist == NULL) {
			UniquePointer<PoolArena> newArena(new PoolArena(m_ItemSize, m_ItemsNumber));
			//printf(">> List is empty allocating more Start at = %d\n", newArena->GetStorage());
			newArena->SetNext(::std::move(m_Arena));
			m_Arena = newArena;
			m_Freelist = m_Arena->GetStorage();
		}

		typename PoolArena::PoolItem* item = m_Freelist;
		m_Freelist = item->GetNext();
		//printf("Giving adress = %d\n", item);
		void* result = (void*) item;
		return result;
	}

	template<typename T, typename... Args>
	T* Allocate(Args&&... args)
	{
		T* result = (T*) this->Allocate();
		new (result) T(::std::forward<Args>(args)...);
		return result;
	}

	template<typename T>
	T* Allocate()
	{
		T* result = (T*) this->Allocate();
		return result;
	}

	void Deallocate(void* ptr)
	{
		ASSERTF((ptr == NULL), "Can't destroy a null pointer...");
		#if !defined(_DEBUG) || defined(NDEBUG)
				if (ptr == NULL) return;
		#endif

		typename PoolArena::PoolItem* current_item = PoolArena::PoolItem::StorageToItem(ptr);
		//printf(">> Dealloacting : %d | Free List : %d\n", current_item, m_Freelist);
		current_item->SetNext(m_Freelist);
		m_Freelist = current_item;
	}

	template<typename T>
	void Deallocate(T* ptr)
	{
		ASSERTF((ptr == NULL), "Can't destroy a null pointer...");
		#if !defined(_DEBUG) || defined(NDEBUG)
				if (ptr == NULL) return;
		#endif

		Utils::Destroy<T>(ptr);
		this->Deallocate((void*)ptr);
	}

private:
	uint32 m_ItemSize, m_ItemsNumber;
	UniquePointer<PoolArena> m_Arena;
	typename PoolArena::PoolItem* m_Freelist;
};

typedef MultiPoolAllocator MultiPoolAlloc;

TRE_NS_END