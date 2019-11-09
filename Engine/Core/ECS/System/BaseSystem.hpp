#pragma once

#include <Core/Misc/Defines/Common.hpp>
#include <Core/DataStructure/Vector/Vector.hpp>
#include <Core/ECS/Component/BaseComponent.hpp>
#include <Core/DataStructure/Tuple/Pair.hpp>
#include <Core/DataStructure/Bitset/Bitset.hpp>

TRE_NS_START

class BaseSystem
{
public:
	using SystemComponent = ComponentTypeID;

	enum {
		FLAG_OPTIONAL = 1,
	};

	BaseSystem() {}

	virtual void UpdateComponents(float delta, BaseComponent** components) {};

	virtual void UpdateEntities(float delta, IEntity& entity) {};

	const SystemComponent* GetSystemComponents() const
	{
		return m_Components;
	}

	uint32 GetComponentsCount() const { return m_ComponentsCount; }

	bool IsValid();

	void AddEntity(IEntity* entity);

	void RemoveEntity(IEntity* entity);

	const Vector<IEntity*>& GetEntities() const { return m_Entities; }

	Vector<IEntity*>& GetEntities() { return m_Entities; }

	usize GetEntitiesCount() { return m_Entities.Size(); }

	void RegisterSystem();

	bool DoesContainEntity(IEntity* entity);
protected:
	BaseSystem(SystemComponent* comps, uint32 comp_count = 0);

protected:
	Vector<IEntity*> m_Entities;
	Bitset m_Signature;
	SystemComponent* m_Components;
	uint32 m_ComponentsCount;
	
	friend class ECS;
};

template<typename... Components>
class System : public BaseSystem
{
protected:
	CONSTEXPR static uint32 COMPONENT_CPACITY = sizeof...(Components);
	
	System() : 
		m_Components{ static_cast<SystemComponent>(Components::ID)... },
		BaseSystem(m_Components, COMPONENT_CPACITY)
	{
		uint32 size = COMPONENT_CPACITY;

		while (size--) {
			m_Signature.Set(m_Components[size], true);
		}
	}

private:
	SystemComponent m_Components[COMPONENT_CPACITY];
};

TRE_NS_END
