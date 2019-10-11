#pragma once
#include <Core/Misc/Defines/Common.hpp>
#include <Core/DataStructure/Vector/Vector.hpp>
#include <Core/ECS/System/BaseSystem.hpp>

TRE_NS_START

class SystemList
{
public:
	bool RemoveSystem(BaseSystem& system);
	bool AddSystem(BaseSystem* system);

	FORCEINLINE usize GetSize() { return m_Systems.Size(); }
	FORCEINLINE BaseSystem* operator[](uint32 index) { return m_Systems[index]; }
private:
	Vector<BaseSystem*> m_Systems;
};

TRE_NS_END