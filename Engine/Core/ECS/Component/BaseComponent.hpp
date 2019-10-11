#pragma once

#include <Core/Misc/Defines/Common.hpp>
#include <Core/DataStructure/Vector/Vector.hpp>
#include <Core/DataStructure/Tuple/Tuple.hpp>

TRE_NS_START

typedef class IEntity* EntityHandle;
typedef uint32 ComponentID;
typedef uint32 ComponentTypeID;
typedef uint32(*ComponentCreateFunction)(Vector<uint8>&, EntityHandle, struct BaseComponent*);
typedef void(*ComponentDeleteFunction)(struct BaseComponent*);

struct BaseComponent
{
private:
	static Vector<Tuple<ComponentCreateFunction, ComponentDeleteFunction, uint32>>* s_ComponentsTypes;

	friend class ECS;

	EntityHandle m_Entity = NULL;
public:	
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
	static uint32 CreateComponent(Vector<uint8>& memory, EntityHandle entity, BaseComponent* comp)
	{
		uint32 index = (uint32) memory.Size();
		memory.Resize(index + Component::SIZE);
		Component* component = new(&memory[index]) Component(*(Component*)comp);
		component->m_Entity = entity;
		return index;
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
	static const ComponentCreateFunction CREATE_FUNCTION;
	static const ComponentDeleteFunction FREE_FUNCTION;
	static const usize SIZE;
	static const ComponentTypeID ID;
};

template<typename T>
const ComponentTypeID Component<T>::ID = BaseComponent::RegisterComponentType(BaseComponent::CreateComponent<T>, BaseComponent::DeleteComponent<T>,  sizeof(T));

template<typename T>
const usize Component<T>::SIZE = sizeof(T);

TRE_NS_END