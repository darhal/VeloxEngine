#include "ECS.hpp"

TRE_NS_START

World ECS::s_Worlds[ECS::MAX_WORLDS];
uint32 ECS::s_WorldCount = 1; // Default world

World& ECS::CreateWorld()
{
	s_Worlds[s_WorldCount].m_WorldId = s_WorldCount;;
	return s_Worlds[s_WorldCount++];
}

World& ECS::GetWorld(uint32 id)
{
	ASSERTF(id >= s_WorldCount, "Invalid usage of ECS::GetWorld() index is out of bound.");
	return s_Worlds[id];
}

void ECS::DestroyWorld(uint32 id)
{
	s_Worlds[s_WorldCount--].~World();
}

void ECS::ShutDown()
{
	for (uint32 i = 0; i < s_WorldCount; i++)
		DestroyWorld(i);
}

TRE_NS_END