#pragma once

#include <Core/Misc/Defines/Common.hpp>
#include <Core/DataStructure/Bitset/Bitset.hpp>
#include <Core/DataStructure/Vector/Vector.hpp>
#include <Core/DataStructure/PackedArray/PackedArray.hpp>
#include <Core/DataStructure/HashMap/HashMap.hpp>
#include <Core/ECS/Archetype/Archetype.hpp>
#include <Core/ECS/Entity/Entity.hpp>

TRE_NS_START

class Entity;

class EntityManager
{
public:
	typedef PackedArray<Archetype> ArchetypeContainer;
public:
	EntityManager(class World* world);

	// Entities :
	FORCEINLINE Entity& CreateEntity();

	template<typename... Components>
	FORCEINLINE Entity& CreateEntityWithComponents(const Components&... components);

	Entity& CreateEntity(BaseComponent** components, const ComponentTypeID* componentIDs, usize numComponents);

	FORCEINLINE Entity& GetEntityByID(uint32 id);

	void DeleteEntity(EntityHandle);

	// Components : 
	template<typename Component>
	FORCEINLINE Component* AddComponent(Entity& entity, Component* component);

	template<typename Component>
	FORCEINLINE bool RemoveComponent(Entity& entity);

	template<typename Component>
	FORCEINLINE Component* GetComponent(const Entity& entity);

	template<typename Component>
	Vector<Component*> GetAllComponents(ComponentTypeID id);

	Vector<BaseComponent*> GetAllComponents(ComponentTypeID id);

	Map<ComponentTypeID, Vector<BaseComponent*>> GetAllComponentsMatchSignture(const Bitset& signature);

	// Archetypes :
	Archetype& CreateArchetype(const Bitset& signature);

	Archetype& GetOrCreateArchetype(const Bitset& sig);

	Archetype* GetArchetype(const Bitset& sig);

	// World :
	World& GetWorld();

private:
	BaseComponent* AddComponentInternal(Entity& entity, uint32 component_id, BaseComponent* component);
	bool RemoveComponentInternal(Entity& entity, uint32 component_id);
	BaseComponent* GetComponentInternal(const Entity& entity, uint32 component_id);

private:
	Vector<Entity> m_Entities;
	ArchetypeContainer m_Archetypes;
	HashMap<Bitset, uint32> m_SigToArchetypes;
	World* m_World;
};

FORCEINLINE Entity& EntityManager::CreateEntity()
{
	// Adding empty component we dont need any archetype for this
	return m_Entities.EmplaceBack(this, (uint32)m_Entities.Size());
}

template<typename... Components>
FORCEINLINE Entity& EntityManager::CreateEntityWithComponents(const Components&... components)
{
	CONSTEXPR usize numComponents = sizeof...(components);
	BaseComponent* components_arr[numComponents] = { (BaseComponent*)(&components)... };
	ComponentTypeID component_ids[numComponents] = { Components::ID... };
	return this->CreateEntity(components_arr, component_ids, numComponents);
}

FORCEINLINE Entity& EntityManager::GetEntityByID(uint32 id)
{
	ASSERTF(id >= m_Entities.Size(), "Invalid usage of 'GetEntityByID', invalid ID is supplied.");
	return m_Entities[id];
}

template<typename Component>
FORCEINLINE Component* EntityManager::AddComponent(Entity& entity, Component* component)
{
	return static_cast<Component*>(AddComponentInternal(entity, Component::ID, component));
}

template<typename Component>
FORCEINLINE bool EntityManager::RemoveComponent(Entity& entity)
{
	return RemoveComponentInternal(entity, Component::ID);
}

template<typename Component>
FORCEINLINE Component* EntityManager::GetComponent(const Entity& entity)
{
	return (Component*)GetComponentInternal(entity, Component::ID);
}


template<typename Component>
Vector<Component*> EntityManager::GetAllComponents(ComponentTypeID id)
{
	return this->GetAllComponents(Component::ID);
}

TRE_NS_END