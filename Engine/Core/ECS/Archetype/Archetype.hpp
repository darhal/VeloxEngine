#pragma once

#include <Core/Misc/Defines/Common.hpp>
#include <Core/DataStructure/Bitset/Bitset.hpp>
#include <Core/DataStructure/HashMap/Map.hpp>
#include <Core/DataStructure/Vector/Vector.hpp>
#include <Core/ECS/Component/BaseComponent.hpp>
#include <Core/ECS/Archetype/Chunk/ArchetypeChunk.hpp>
#include <Core/ECS/Entity/Entity.hpp>
#include <Core/DataStructure/Utils/Utils.hpp>

TRE_NS_START

class Archetype
{
public:
	Archetype(const Bitset& bitset, const Vector<ComponentTypeID>& ids);

	Archetype(const Vector<ComponentTypeID>& ids);

	Archetype(const Bitset& bitset);

	ArchetypeChunk* AddEntity(Entity& entity);
	
	 ~Archetype();

	void AddComponentType(ComponentTypeID id);

	ArchetypeChunk* AddEntityComponents(Entity& entity, BaseComponent** components, const ComponentTypeID* componentIDs, usize numComponents);

	ArchetypeChunk* GetAllocationChunk();

	FORCEINLINE uint32 GetID() { return m_Id; }

	FORCEINLINE const Bitset& GetSignature() const { return m_Signature; }

	FORCEINLINE const Map<ComponentTypeID, uint32>& GetTypesBufferMarker() { return m_TypesToBuffer; }

	FORCEINLINE bool IsEmpty() { return m_OccupiedChunks == NULL; }

	FORCEINLINE uint32 GetComponentsTypesCount() const { return m_TypesCount; }

private:
	Map<ComponentTypeID, uint32> m_TypesToBuffer;
	Bitset m_Signature;
	ArchetypeChunk* m_FreeChunks;
	ArchetypeChunk* m_OccupiedChunks;
	uint32 m_TypesCount;
	uint32 m_ComponentsArraySize;
	uint32 m_Id;

	friend class ECS;
	friend class ArchetypeChunk;

	ArchetypeChunk* GenerateChunk();
	ArchetypeChunk* GetLastOccupiedChunk();
	void PushFreeChunk(ArchetypeChunk* chunk);
	FORCEINLINE void SetID(uint32 id) { m_Id = id; }
};



TRE_NS_END