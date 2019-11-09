#include "BaseSystem.hpp"
#include <Core/ECS/Manager/ECS.hpp>
#include <Core/ECS/Entity/IEntity/IEntity.hpp>

TRE_NS_START

BaseSystem::BaseSystem(SystemComponent* comps, uint32 comp_count)
	: m_Entities(), m_Signature(BaseComponent::GetComponentsCount()), m_Components(comps), m_ComponentsCount(comp_count)
{
}

void BaseSystem::RegisterSystem()
{
	ECS::RegisterSystem(m_Signature, this);
}

void BaseSystem::AddEntity(IEntity* entity) 
{
	entity->m_InternalId = (EntityID) m_Entities.Size();
	m_Entities.EmplaceBack(entity);
}

void BaseSystem::RemoveEntity(IEntity* entity)
{
	uint32 destIndex = entity->m_InternalId;
	ssize srcIndex = (ssize) m_Entities.Size() - 1;

	if (srcIndex > 0) {
		m_Entities[destIndex] = m_Entities[srcIndex];
		m_Entities[destIndex]->m_InternalId = destIndex;
	}

	m_Entities.PopBack();
}

bool BaseSystem::DoesContainEntity(IEntity* entity)
{
	uint32 internal_id = entity->m_InternalId;
	return (m_Entities.Size() > internal_id) && (m_Entities[entity->m_InternalId] == entity);
}

bool BaseSystem::IsValid()
{
	/*for (uint32 i = 0; i < this->GetComponentsCount(); i++) {
		if ((m_Components[i].second & BaseSystem::FLAG_OPTIONAL) == 0) {
			return true;
		}
	}*/
	return true;
	// return false;
}

TRE_NS_END