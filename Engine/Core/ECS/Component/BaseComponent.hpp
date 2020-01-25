#pragma once

#include <Core/Misc/Defines/Common.hpp>
#include <Core/DataStructure/Vector/Vector.hpp>
#include <Core/DataStructure/Tuple/Tuple.hpp>
#include <Core/ECS/Common.hpp>
#include <Core/DataStructure/Utils/Utils.hpp>
#include <Core/DataStructure/StaticBitset/StaticBitset.hpp>

TRE_NS_START

struct BaseComponent
{
public:
	FORCEINLINE static usize GetComponentsCount() { return s_ComponentsTypes->Size(); }

private:
	friend class ECS;
	friend class Archetype;
	friend class ArchetypeChunk;
	friend class EntityManager;
	friend class BaseSystem;

	static Vector<Tuple<ComponentCreateFunction, ComponentDeleteFunction, uint32>>* s_ComponentsTypes;
protected:	
	static uint32 RegisterComponentType(ComponentCreateFunction createfn, ComponentDeleteFunction freefn, uint32 size);

	FORCEINLINE static ComponentCreateFunction GetTypeCreateFunction(uint32 id)
	{
		return (*s_ComponentsTypes)[id].Get<0>();
	}

	FORCEINLINE static ComponentDeleteFunction GetTypeDeleteFunction(uint32 id)
	{
		return (*s_ComponentsTypes)[id].Get<1>();
	}

	FORCEINLINE static uint32 GetTypeSize(uint32 id)
	{
		return (*s_ComponentsTypes)[id].Get<2>();
	}

	FORCEINLINE static bool IsValidTypeID(uint32 id)
	{
		return id < s_ComponentsTypes->Size();
	}

	template<typename Component>
	static BaseComponent* CreateComponent(uint8* memory, BaseComponent* comp)
	{
		Component* component = new (memory) Component(*(Component*)comp);
		return (BaseComponent*) component;
	}

	template<typename Component>
	static void DeleteComponent(BaseComponent* comp)
	{
		Component* component = (Component*) comp;
		component->~Component();
	}
};

template<typename T>
struct Component : public BaseComponent
{
public:
	static const usize SIZE;
	static const ComponentTypeID ID;

	FORCEINLINE static usize GetTypeID() { return ID; }
private:
	static const ComponentCreateFunction CREATE_FUNCTION;
	static const ComponentDeleteFunction FREE_FUNCTION;
	// static CONSTEXPR ComponentTypeID _ID = Utils::NextID();
};

template<typename T>
const ComponentTypeID Component<T>::ID = BaseComponent::RegisterComponentType(BaseComponent::CreateComponent<T>, BaseComponent::DeleteComponent<T>,  sizeof(T));

template<typename T>
const usize Component<T>::SIZE = sizeof(T);

TRE_NS_END