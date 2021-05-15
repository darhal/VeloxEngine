#pragma once

#include <Core/Misc/Defines/Common.hpp>
#include <Core/DataStructure/Bitset/Bitset.hpp>
#include <Core/DataStructure/HashMap/Map.hpp>
#include <Core/DataStructure/Utils/Utils.hpp>
#include <Core/ECS/Component/BaseComponent.hpp>

TRE_NS_START

class EntityManager;

class Archetype
{
public:
	template<typename PointerType>
	class GenericIterator;

	typedef GenericIterator<ArchetypeChunk> Iterator;
	typedef GenericIterator<const ArchetypeChunk> CIterator;
public:
	Archetype(EntityManager* manager, const Bitset& bitset, const Vector<ComponentTypeID>& ids);

	Archetype(EntityManager* manager, const Vector<ComponentTypeID>& ids);

	Archetype(EntityManager* manager, const Bitset& bitset);

	ArchetypeChunk* AddEntity(Entity& entity);
	
	 ~Archetype();

	void AddComponentType(ComponentTypeID id);

	ArchetypeChunk* AddEntityComponents(Entity& entity, BaseComponent** components, const ComponentTypeID* componentIDs, usize numComponents);

	ArchetypeChunk* GetAllocationChunk();

	ArchetypeChunk* GetLastOccupiedChunk();

	EntityManager& GetEntityManager() const;

	FORCEINLINE uint32 GetID() { return m_Id; }

	FORCEINLINE const Bitset& GetSignature() const { return m_Signature; }

	FORCEINLINE const Map<ComponentTypeID, uint32>& GetTypesBufferMarker() const { return m_TypesToBuffer; }

	FORCEINLINE bool IsEmpty() const { return m_OccupiedChunks == NULL; }

	FORCEINLINE uint32 GetComponentsTypesCount() const { return m_TypesCount; }

	bool HasComponentType(ComponentTypeID id) const;

	template<typename Component>
	bool HasComponentType() const { return HasComponentType(Component::ID); }

	Iterator begin() { return Iterator(m_OccupiedChunks); }
	Iterator end() { return Iterator(NULL); }

	const Iterator begin() const { return Iterator(m_OccupiedChunks); }
	const Iterator end() const { return Iterator(NULL); }

	const CIterator cbegin() const { return CIterator(m_OccupiedChunks); }
	const CIterator cend() const { return CIterator(NULL); }
private:
	Map<ComponentTypeID, uint32> m_TypesToBuffer;
	Bitset m_Signature;
	EntityManager* m_Manager;
	ArchetypeChunk* m_OccupiedChunks;
	ArchetypeChunk* m_FreeChunks;
	uint32 m_TypesCount;
	uint32 m_ComponentsArraySize;
	uint32 m_Id;

	friend class EntityManager;
	friend class ArchetypeChunk;

	ArchetypeChunk* GenerateChunk();

	void PushFreeChunk(ArchetypeChunk* chunk);

	FORCEINLINE void SetID(uint32 id) { m_Id = id; }

public:
	template<typename DataType>
	class GenericIterator
	{
	public:
		GenericIterator(DataType* node) noexcept : m_CurrentNode(node) {}
		bool operator!=(const Iterator& iterator) { return m_CurrentNode != iterator.m_CurrentNode; }

		DataType& operator*() { return *m_CurrentNode; }
		const DataType& operator*() const { return (*m_CurrentNode); }

		DataType* operator->() { return m_CurrentNode; }
		const DataType* operator->() const { return m_CurrentNode; }

		GenericIterator& operator=(DataType* node) {
			this->m_CurrentNode = node;
			return *this;
		}

		GenericIterator& operator++() {
			if (m_CurrentNode)
				m_CurrentNode = m_CurrentNode->GetNextChunk();
			return *this;
		}

		GenericIterator operator++(int) {
			GenericIterator iterator = *this;
			++*this;
			return iterator;
		}

	private:
		DataType* m_CurrentNode;
	};
};



TRE_NS_END