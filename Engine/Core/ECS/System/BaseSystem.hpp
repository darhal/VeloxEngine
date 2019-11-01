#pragma once

#include <Core/Misc/Defines/Common.hpp>
#include <Core/DataStructure/Vector/Vector.hpp>
#include <Core/ECS/Component/BaseComponent.hpp>
#include <Core/DataStructure/Tuple/Pair.hpp>

TRE_NS_START

class BaseSystem
{
public:
	using SystemComponent = Pair<ComponentTypeID, uint32>;

	enum{
		FLAG_OPTIONAL = 1,
	};

	BaseSystem() {}

	virtual void UpdateComponents(float delta, BaseComponent** components) {};

	/*const Vector<uint32>& GetComponentTypes() const
	{
		return m_ComponentTypes;
	}

	const Vector<uint32>& GetComponentFlags() const
	{
		return m_ComponentFlags;
	}*/

	const SystemComponent* GetSystemComponents() const
	{
		return m_Components;
	}

	uint32 GetComponentsCount() const { return m_ComponentsCount; }

	bool IsValid();
protected:

	void AddComponentType(ComponentTypeID componentType, uint32 componentFlag = 0)
	{
		ASSERTF(m_ComponentsCount >= m_ComponentsSize, "Can't add new component type to the system, size limit exceded please consider incrementing the size current (count: %d, limit : %d)", m_ComponentsCount, m_ComponentsSize);
		new (m_Components + (m_ComponentsCount++)) SystemComponent(componentType, componentFlag);
	}

	BaseSystem(SystemComponent* comps, uint32 comp_size, uint32 comp_count = 0)
		: m_Components(comps), m_ComponentsSize(comp_size), m_ComponentsCount(comp_count)
	{
	}

private:	
	SystemComponent* m_Components;
	uint32 m_ComponentsSize;
	uint32 m_ComponentsCount;

	friend class ECS;
};

template<typename T, uint32 N = 8>
class System : public BaseSystem
{
protected:
	typedef BaseSystem::SystemComponent SComponent;

	CONSTEXPR static uint32 COMPONENT_CPACITY = N;
	
	System() : BaseSystem(m_Components, COMPONENT_CPACITY)
	{}

	template<typename... Args>
	System(Args&&... args) : 
		BaseSystem(m_Components, COMPONENT_CPACITY, sizeof...(Args)), 
		m_Components{ static_cast<SystemComponent>(std::forward<Args>(args))... }
	{
	}

	friend T;
private:
	SystemComponent m_Components[COMPONENT_CPACITY];
};

TRE_NS_END
