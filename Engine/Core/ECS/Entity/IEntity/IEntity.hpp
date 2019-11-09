#pragma once

#include "Core/Misc/Defines/Common.hpp"
#include <Core/ECS/Component/BaseComponent.hpp>
#include <Core/DataStructure/Vector/Vector.hpp>
#include <Core/DataStructure/Tuple/Pair.hpp>

TRE_NS_START

typedef uint32 EntityID;

class IEntity
{
public:

	IEntity() : m_Components(), m_Signature(BaseComponent::GetComponentsCount()),
		m_Id(0), m_InternalId(0)
	{}

	typedef uint32 IndexInMemory;

	virtual ~IEntity() = default;

	FORCEINLINE const EntityID GetEntityID() const { return this->m_Id; }

	template<typename Component>
	FORCEINLINE Component* AddComponent(Component& comp);

	template<typename Component, typename... Args>
	FORCEINLINE Component* CreateComponent(Args&&... args);

	template<typename Component>
	FORCEINLINE bool RemoveComponent();

	template<typename Component>
	FORCEINLINE BaseComponent* GetComponent();

	const Vector<Pair<ComponentTypeID, IndexInMemory>>& GetComponentsData() const { return m_Components; }
protected:
	Vector<Pair<ComponentTypeID, IndexInMemory>> m_Components;
	Bitset m_Signature;
	EntityID m_Id;
	uint32 m_InternalId;
	
	friend class ECS;
	friend class BaseSystem;

private:
	FORCEINLINE void AddComponent(ComponentTypeID type_id, IndexInMemory index_in_mem);
};

FORCEINLINE void IEntity::AddComponent(ComponentTypeID type_id, IndexInMemory index_in_mem)
{
	m_Components.EmplaceBack(type_id, index_in_mem);
}

template<typename Component>
FORCEINLINE Component* IEntity::AddComponent(Component& comp)
{
	return ECS::AddComponent(*this, &comp);
}

template<typename Component, typename... Args>
FORCEINLINE Component* IEntity::CreateComponent(Args&&... args)
{
	Component component(std::forward<Args>(args)...);
	return ECS::AddComponent(*this, &component);
}

template<typename Component>
FORCEINLINE bool IEntity::RemoveComponent()
{
	return ECS::RemoveComponent<Component>(*this);
}

template<typename Component>
FORCEINLINE BaseComponent* IEntity::GetComponent()
{
	return ECS::GetComponent<Component>(*this);
}

TRE_NS_END