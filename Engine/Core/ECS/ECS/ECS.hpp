#pragma once

#include <Core/Misc/Defines/Common.hpp>
#include <Core/ECS/World/World.hpp>

TRE_NS_START

class ECS
{
public:
	enum { DefaultWorld = 0 };

	static World& CreateWorld();

	static World& GetWorld(uint32 id);

	static void DestroyWorld(uint32 id);

	static void ShutDown();
private:
	CONSTEXPR static uint32 MAX_WORLDS = 5;

	static World s_Worlds[MAX_WORLDS];
	static uint32 s_WorldCount;
};

TRE_NS_END