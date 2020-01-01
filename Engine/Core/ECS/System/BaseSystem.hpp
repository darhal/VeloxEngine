#pragma once

#include <Core/Misc/Defines/Common.hpp>
#include <Core/DataStructure/HashMap/Map.hpp>
#include <Core/DataStructure/Vector/Vector.hpp>
#include <Core/ECS/Component/BaseComponent.hpp>
#include <Core/DataStructure/Tuple/Pair.hpp>
#include <Core/DataStructure/Bitset/Bitset.hpp>

TRE_NS_START

class Archetype;

class BaseSystem
{
public:
	enum {
		FLAG_OPTIONAL = 1,
	};

	virtual void UpdateComponents(float delta, ComponentTypeID comp_type, BaseComponent* components) {};

	virtual const ComponentTypeID* const GetComponentsTypes() const = 0;

	virtual const Bitset& GetSignature() const = 0;

	virtual uint32 GetComponentsCount() const = 0;

	Archetype* GetArchetype() { return m_Archetype; }

	void SetArchetype(Archetype* archetype) { m_Archetype = archetype; }
protected:
	BaseSystem() : m_Archetype(NULL)
	{};

	Archetype* m_Archetype;
	friend class ECS;
};

template<typename... Components>
class System : public BaseSystem
{
protected:
	CONSTEXPR static uint32 COMPONENTS_COUNT = sizeof...(Components);

	System() : 
		m_ComponentsTypes{ static_cast<ComponentTypeID>(Components::ID)... },
		m_Signature(BaseComponent::GetComponentsCount())
	{
		uint32 size = COMPONENTS_COUNT;

		while (size--) {
			m_Signature.Set(m_ComponentsTypes[size], true);
		}
	}

	FORCEINLINE const ComponentTypeID* const GetComponentsTypes() const final { return m_ComponentsTypes;  };

	FORCEINLINE const Bitset& GetSignature() const final { return m_Signature; };

	FORCEINLINE uint32 GetComponentsCount() const final { return COMPONENTS_COUNT; };
private:
	ComponentTypeID m_ComponentsTypes[COMPONENTS_COUNT];
	Bitset m_Signature;
};

TRE_NS_END
