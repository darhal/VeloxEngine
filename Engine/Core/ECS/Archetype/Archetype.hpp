#pragma once

#include <Core/Misc/Defines/Common.hpp>
#include <Core/DataStructure/Bitset/Bitset.hpp>
#include <Core/DataStructure/HashMap/Map.hpp>
#include <Core/DataStructure/Vector/Vector.hpp>
#include <Core/ECS/Component/BaseComponent.hpp>
#include <Core/ECS/Entity/IEntity/IEntity.hpp>
#include <Core/DataStructure/Utils/Utils.hpp>

TRE_NS_START

class Archetype
{
public:
	Archetype() :
		m_ComponentsBuffer{},
		m_Signature(BaseComponent::GetComponentsCount()),
		m_EntitiesCount(0), m_Id(-1)
	{
	}

	Archetype(Archetype&& other) :
		m_ComponentsBuffer(std::move(other.m_ComponentsBuffer)),
		m_Signature(std::move(other.m_Signature)),
		m_EntitiesCount(other.m_EntitiesCount), m_Id(other.m_Id)
	{
	}

	uint32 AddEntityComponents(IEntity& entity, BaseComponent** components, const ComponentTypeID* componentIDs, usize numComponents);

	uint32 RemoveEntityComponents(IEntity& entity);

	uint32 DestroyEntityComponents(IEntity& entity);

	void DestroyComponent(IEntity& entity, ComponentTypeID id);

	FORCEINLINE Vector<uint8>& AddComponentType(ComponentTypeID componentID);

	FORCEINLINE void RemoveComponentType(ComponentTypeID componentID);

	FORCEINLINE uint32 AddEntity(IEntity& entity);

	FORCEINLINE BaseComponent* UpdateComponentMemory(IEntity& entity, BaseComponent* component, ComponentTypeID component_id);

	FORCEINLINE BaseComponent* AddComponentToEntity(IEntity& entity, BaseComponent* component, ComponentTypeID component_id);

	FORCEINLINE BaseComponent* GetComponent(const IEntity& entity, ComponentTypeID component_id) const;
	
	FORCEINLINE Vector<uint8>& GetComponentBuffer(ComponentTypeID id) { return m_ComponentsBuffer[id]; }

	FORCEINLINE Map<ComponentTypeID, Vector<uint8>>& GetComponentsBuffer() { return m_ComponentsBuffer; }

	FORCEINLINE const Bitset& GetSignature() const { return m_Signature; }

	FORCEINLINE uint32 GetEntitesCount() const { return m_EntitiesCount; }

	FORCEINLINE uint32 GetID() const { return m_Id; }
private:
	uint32 ReseveEntity();

	FORCEINLINE BaseComponent* GetComponentInternal(const IEntity& entity, const Vector<uint8>& buffer, ComponentTypeID component_id) const;

	uint32 RemoveEntityComponentsInternal(uint32 index);

	void RemoveComponentInternal(Vector<uint8>& components_buffer, ComponentTypeID id, uint32 mem_index);

	uint32 DestroyEntityComponentsInternal(uint32 index);

	void DestroyComponentInternal(Vector<uint8>& components_buffer, ComponentTypeID id, uint32 mem_index);

	void DestroyComponentHelper(Vector<uint8>& components_buffer, ComponentTypeID id, uint32 mem_index);

	BaseComponent* AddComponentToEntityInternal(IEntity& entity, Vector<uint8>& buffer, BaseComponent* component, ComponentTypeID component_id);

	BaseComponent* UpdateComponentMemoryInternal(uint32 internal_id, IEntity& entity, BaseComponent* component, ComponentTypeID component_id);

	FORCEINLINE void SetID(uint32 id) { m_Id = id; }

	IEntity* GetEntity(uint32 index);
private:
	Map<ComponentTypeID, Vector<uint8>> m_ComponentsBuffer;
	Bitset m_Signature;
	uint32 m_EntitiesCount; 
	uint32 m_Id;

	friend class ECS;
};

FORCEINLINE Vector<uint8>& Archetype::AddComponentType(ComponentTypeID componentID)
{
	m_Signature.Set(componentID, true);
	return m_ComponentsBuffer.Emplace(componentID, Vector<uint8>());
}

FORCEINLINE void Archetype::RemoveComponentType(ComponentTypeID componentID)
{
	m_Signature.Set(componentID, false);
	m_ComponentsBuffer.Remove(componentID);
}

FORCEINLINE BaseComponent* Archetype::GetComponent(const IEntity& entity, ComponentTypeID component_id) const
{
	return GetComponentInternal(entity, m_ComponentsBuffer[component_id], component_id);
}

FORCEINLINE BaseComponent* Archetype::GetComponentInternal(const IEntity& entity, const Vector<uint8>& buffer, ComponentTypeID component_id) const
{
	return (BaseComponent*)(&(buffer[0]) + (entity.m_InternalId * BaseComponent::GetTypeSize(component_id)));
}

FORCEINLINE uint32 Archetype::AddEntity(IEntity& entity)
{
	return entity.AttachToArchetype(*this, this->ReseveEntity());
}

FORCEINLINE BaseComponent* Archetype::UpdateComponentMemory(IEntity& entity, BaseComponent* component, ComponentTypeID component_id)
{
	return this->UpdateComponentMemoryInternal(entity.m_InternalId, entity, component, component_id);
}

FORCEINLINE BaseComponent* Archetype::AddComponentToEntity(IEntity& entity, BaseComponent* component, ComponentTypeID component_id)
{
	return this->AddComponentToEntityInternal(entity, m_ComponentsBuffer[component_id], component, component_id);
}

TRE_NS_END