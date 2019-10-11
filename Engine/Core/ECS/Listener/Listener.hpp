#pragma once

#include <Core/Misc/Defines/Common.hpp>
#include <Core/ECS/Component/BaseComponent.hpp>

TRE_NS_START

class Listener
{
	virtual void OnEntityCreation(EntityHandle handle) = 0;

	virtual void OnEntityRemove(EntityHandle handle) = 0;
	
	virtual void OnAddComponent(EntityHandle handle, ComponentID id) = 0;

	virtual void OnRemoveComponenet(EntityHandle handle, ComponentID id) = 0;

	const Vector<uint32>& GetComponentsID() { return m_ComponentsID; }
protected:
	void AddComponentID(uint32 id) {
		m_ComponentsID.EmplaceBack(id);
	}
private:
	Vector<uint32> m_ComponentsID;
};

TRE_NS_END