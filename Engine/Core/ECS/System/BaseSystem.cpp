#include "BaseSystem.hpp"

TRE_NS_START

bool BaseSystem::IsValid()
{
	for (uint32 i = 0; i < this->GetComponentsCount(); i++) {
		if ((m_Components[i].second & BaseSystem::FLAG_OPTIONAL) == 0) {
			return true;
		}
	}

	return false;
}

TRE_NS_END