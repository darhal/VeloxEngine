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
	IEntity() : m_ArchetypeId(0), m_Id(0), m_InternalId(0)
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
protected:
	EntityID m_Id;
	EntityID m_ArchetypeId;	
	EntityID m_InternalId;
	
	friend class ECS;
	friend class BaseSystem;
	friend class Archetype;

private:
	uint32 AttachToArchetype(const class Archetype& arcehtype, EntityID internal_id);
};

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