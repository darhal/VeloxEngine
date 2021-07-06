#pragma once

#include <Legacy/Misc/Defines/Common.hpp>
#include <Legacy/DataStructure/Map.hpp>
#include <Legacy/DataStructure/Vector.hpp>
#include <Legacy/ECS/Component/BaseComponent.hpp>
#include <Legacy/DataStructure/Pair.hpp>
#include <Legacy/DataStructure/Bitset.hpp>
#include <Legacy/ECS/ComponentGroup/ComponentGroup.hpp>
#include <Legacy/ECS/CommandRecord/CommandRecord.hpp>

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
