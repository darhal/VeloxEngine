#pragma once

#include <Core/Misc/Defines/Common.hpp>
#include <Core/DataStructure/HashMap/Map.hpp>
#include <Core/DataStructure/Vector/Vector.hpp>
#include <Core/ECS/Component/BaseComponent.hpp>
#include <Core/DataStructure/Tuple/Pair.hpp>
#include <Core/DataStructure/Bitset/Bitset.hpp>
#include <Core/ECS/ComponentGroup/ComponentGroup.hpp>
#include <Core/ECS/CommandRecord/CommandRecord.hpp>

TRE_NS_START

class Archetype;
class SystemList;

class BaseSystem
{
public:
	virtual void OnUpdate(float delta) = 0;

	const ComponentGroup& GetComponentGroup() const { return m_ComponentGroup; }

	virtual ~BaseSystem() = default;

protected:
	BaseSystem() {};

	void FlushCommandsRecord() { m_CommandRecord.Flush(); }

	ComponentGroup m_ComponentGroup;
	CommandRecord m_CommandRecord;
	SystemList* m_SystemList;
private:
	void AddToList(SystemList* list);

	void QuerryComponents(EntityManager& manager) { m_ComponentGroup.QuerryArchetypes(manager); }

	ComponentGroup& GetComponentGroup() { return m_ComponentGroup; }

	friend class ECS;
	friend class SystemList;
	friend class EntityManager;
	friend class World;
};


TRE_NS_END
