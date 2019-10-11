#include "BaseSystem.hpp"

TRE_NS_START

bool BaseSystem::IsValid()
{
	for (uint32 i = 0; i < m_ComponentFlags.Size(); i++) {
		if ((m_ComponentFlags[i] & BaseSystem::FLAG_OPTIONAL) == 0) {
			return true;
		}
	}

	return false;
}

TRE_NS_END