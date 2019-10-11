#include "BaseComponent.hpp"

TRE_NS_START

Vector<Tuple<ComponentCreateFunction, ComponentDeleteFunction, uint32>>* BaseComponent::s_ComponentsTypes;

ComponentTypeID BaseComponent::RegisterComponentType(ComponentCreateFunction createfn, ComponentDeleteFunction freefn, uint32 size)
{
	if (s_ComponentsTypes == NULL) {
		s_ComponentsTypes = new Vector<Tuple<ComponentCreateFunction, ComponentDeleteFunction, uint32>>();
	}

	ComponentID componentID = (ComponentID) s_ComponentsTypes->Size();
	s_ComponentsTypes->EmplaceBack(Tuple<ComponentCreateFunction, ComponentDeleteFunction, uint32>(
		createfn, freefn, size)
	);

	return componentID;
}

TRE_NS_END