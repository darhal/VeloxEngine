#pragma once

#include <type_traits>
#include <Core/Misc/Defines/Common.hpp>
#include <Core/DataStructure/Bitset/Bitset.hpp>
#include <Core/DataStructure/Vector/Vector.hpp>
#include <Core/DataStructure/HashMap/Map.hpp>
#include <Core/DataStructure/HashMap/HashMap.hpp>
#include <Core/ECS/Component/BaseComponent.hpp>
#include <Core/ECS/Archetype/Archetype.hpp>
#include <Core/ECS/Entity/IEntity/IEntity.hpp>
#include <Core/ECS/System/BaseSystem.hpp>
#include <Core/ECS/System/SystemList.hpp>

TRE_NS_START

class ECS
{
public:
	~ECS();

	// Entities :
	template<typename Entity, typename... Args>
	static FORCEINLINE Entity* CreateEntity(Args&&... args);

	static EntityHandle CreateEntity(BaseComponent** components, const ComponentTypeID* componentIDs, usize numComponents);

	static void DeleteEntity(EntityHandle);

	// Compnents :
	template<typename Component>
	static FORCEINLINE Component* AddComponent(IEntity& entity, Component* component);

	template<typename Component>
	static FORCEINLINE bool RemoveComponent(IEntity& entity);

	template<typename Component>
	static FORCEINLINE BaseComponent* GetComponent(const IEntity& entity);

	static FORCEINLINE const Vector<uint8>& GetComponentBuffer(ComponentTypeID type_id) { return m_Components[type_id];}

	// Systems : 
	static void UpdateSystems(SystemList& system_list, float delta);
private:
	static Map<ComponentTypeID, Vector<uint8>> m_Components;
	static Vector<IEntity*> m_Entities;
	static Vector<Archetype> m_Archetypes;
	static HashMap<Bitset, uint32> m_SigToArchetypes;

	static BaseComponent* AddComponentInternal(IEntity& entity, uint32 component_id, BaseComponent* component);
	static bool RemoveComponentInternal(IEntity& entity, uint32 component_id);
	static BaseComponent* GetComponentInternal(const Archetype& archetype, const IEntity& entity, uint32 component_id);
	/*static void UpdateSystemWithMultipleComponents(
		SystemList& system_list, uint32 index, float delta, const BaseSystem::SystemComponent* component_types_flags, 
		Vector<BaseComponent*>& component_param, Vector<Vector<uint8>*>& component_arrays
	);
	static uint32 FindLeastCommonComponent(const BaseSystem::SystemComponent* component_types_flags, uint32 N);*/
};

template<typename Entity, typename... Args>
FORCEINLINE Entity* ECS::CreateEntity(Args&&... args)
{
	static_assert(std::is_base_of<IEntity, Entity>::value, "Entity should be derived from IEntity!");

	// Adding empty component we dont need any archetype for this
	Entity* entity = new Entity(std::forward<Args>(args)...);
	entity->m_Id = (EntityID) m_Entities.Size();
	entity->m_ArchetypeId = -1;
	entity->m_InternalId = -1;
	m_Entities.EmplaceBack(entity);
	return entity;
}

template<typename Component>
FORCEINLINE Component* ECS::AddComponent(IEntity& entity, Component* component)
{
	return static_cast<Component*>(ECS::AddComponentInternal(entity, Component::ID, component));
}

template<typename Component>
bool ECS::RemoveComponent(IEntity& entity)
{
	return ECS::RemoveComponentInternal(entity, Component::ID);
}

template<typename Component>
FORCEINLINE BaseComponent* ECS::GetComponent(const IEntity& entity)
{
	return (Component*) ECS::GetComponentInternal(m_Archetypes[m_SigToArchetypes[entity]], entity,  Component::ID);
}

TRE_NS_END