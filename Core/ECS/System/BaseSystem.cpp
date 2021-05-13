#include "BaseSystem.hpp"
#include "SystemList.hpp"
#include <Core/ECS/World/World.hpp>

TRE_NS_START

void BaseSystem::AddToList(SystemList* list)
{
	m_SystemList = list;
	this->QuerryComponents(m_SystemList->m_World->GetEntityManager());
}

TRE_NS_END