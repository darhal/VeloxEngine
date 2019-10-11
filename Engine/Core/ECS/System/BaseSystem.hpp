#pragma once

#include <Core/Misc/Defines/Common.hpp>
#include <Core/DataStructure/Vector/Vector.hpp>
#include <Core/ECS/Component/BaseComponent.hpp>

TRE_NS_START

class BaseSystem
{
public:
	enum
	{
		FLAG_OPTIONAL = 1,
	};

	BaseSystem() {}

	virtual void UpdateComponents(float delta, BaseComponent** components) {};

	const Vector<uint32>& GetComponentTypes() const
	{
		return m_ComponentTypes;
	}

	const Vector<uint32>& GetComponentFlags() const
	{
		return m_ComponentFlags;
	}

	bool IsValid();
protected:

	void AddComponentType(ComponentTypeID componentType, uint32 componentFlag = 0)
	{
		m_ComponentTypes.EmplaceBack(componentType);
		m_ComponentFlags.EmplaceBack(componentFlag);
	}

private:
	Vector<uint32> m_ComponentTypes;
	Vector<uint32> m_ComponentFlags;

	friend class ECS;
};

TRE_NS_END
