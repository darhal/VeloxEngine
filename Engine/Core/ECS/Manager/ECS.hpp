#pragma once

#include <type_traits>
#include <Core/Misc/Defines/Common.hpp>
#include <Core/DataStructure/Bitset/Bitset.hpp>
#include <Core/DataStructure/Vector/Vector.hpp>
#include <Core/DataStructure/PackedArray/PackedArray.hpp>
#include <Core/DataStructure/HashMap/Map.hpp>
#include <Core/DataStructure/HashMap/HashMap.hpp>
#include <Core/ECS/Component/BaseComponent.hpp>
#include <Core/ECS/Archetype/Archetype.hpp>
#include <Core/ECS/Entity/Entity.hpp>
#include <Core/ECS/System/BaseSystem.hpp>
#include <Core/ECS/System/SystemList.hpp>
#include <map>

TRE_NS_START

class ECS
{
public:
	~ECS();

	// Entities :
	static FORCEINLINE Entity& CreateEntity();

	template<typename... Components>
	static FORCEINLINE Entity& CreateEntityWithComponents(const Components&... components);

	static Entity& CreateEntity(BaseComponent** components, const ComponentTypeID* componentIDs, usize numComponents);

	static void DeleteEntity(EntityHandle);

	// Compnents :
	template<typename Component>
	static FORCEINLINE Component* AddComponent(Entity& entity, Component* component);

	template<typename Component>
	static FORCEINLINE bool RemoveComponent(Entity& entity);

	template<typename Component>
	static FORCEINLINE Component* GetComponent(const Entity& entity);

	// Systems : 
	static void UpdateSystems(SystemList& system_list, float delta);

	template<typename... Args>
	static Archetype& CreateArchetype(const Bitset& signature, Args&&... args);

	static Archetype& GetOrCreateArchetype(const Bitset& sig);
private:
	typedef PackedArray<Archetype> ArchetypeContainer;

	static Vector<Entity> m_Entities;
	static ArchetypeContainer m_Archetypes;
	static HashMap<Bitset, uint32> m_SigToArchetypes;

	static BaseComponent* AddComponentInternal(Entity& entity, uint32 component_id, BaseComponent* component);
	static bool RemoveComponentInternal(Entity& entity, uint32 component_id);
	static BaseComponent* GetComponentInternal(const Entity& entity, uint32 component_id);
};

template<typename... Args>
Archetype& ECS::CreateArchetype(const Bitset& signature, Args&&... args)
{
	ArchetypeContainer::Object& archetype_pair = m_Archetypes.Put(signature, std::forward<Args>(args)...);
	Archetype& archetype = archetype_pair.second;
	archetype.SetID(archetype_pair.first);
	m_SigToArchetypes.Emplace(signature, archetype_pair.first);
	return archetype;
}

FORCEINLINE Entity& ECS::CreateEntity()
{
	// Adding empty component we dont need any archetype for this
	return m_Entities.EmplaceBack((EntityID)m_Entities.Size());
}

template<typename... Components>
FORCEINLINE Entity& ECS::CreateEntityWithComponents(const Components&... components)
{
	CONSTEXPR usize numComponents = sizeof...(components);
	BaseComponent* components_arr[numComponents] = { (BaseComponent*)(&components)... };
	ComponentTypeID component_ids[numComponents] = { Components::ID... };
	return ECS::CreateEntity(components_arr, component_ids, numComponents);
}

template<typename Component>
FORCEINLINE Component* ECS::AddComponent(Entity& entity, Component* component)
{
	return static_cast<Component*>(ECS::AddComponentInternal(entity, Component::ID, component));
}

template<typename Component>
bool ECS::RemoveComponent(Entity& entity)
{
	return ECS::RemoveComponentInternal(entity, Component::ID);
}

template<typename Component>
FORCEINLINE Component* ECS::GetComponent(const Entity& entity)
{
	return (Component*) ECS::GetComponentInternal(entity, Component::ID);
}

TRE_NS_END