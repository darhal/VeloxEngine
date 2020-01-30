#pragma once

#include <Core/Misc/Defines/Common.hpp>
#include <Core/ECS/World/World.hpp>
#include <Core/DataStructure/Bitset/Bitset.hpp>
#include <Core/ECS/Component/BaseComponent.hpp>

TRE_NS_START

class ECS
{
public:
	enum { DefaultWorld = 0 };

	static World& CreateWorld();

	static World& GetWorld(uint32 id);

	static void DestroyWorld(uint32 id);

	static void ShutDown();

	template<typename... Components>
	static Bitset GetSignature();
private:
	CONSTEXPR static uint32 MAX_WORLDS = 5;

	static World s_Worlds[MAX_WORLDS];
	static uint32 s_WorldCount;
};

template<typename... Components>
static Bitset ECS::GetSignature()
{
	Bitset sig(BaseComponent::GetComponentsCount());
	CONSTEXPR usize numComponents = sizeof...(Components);
	CONSTEXPR ComponentTypeID component_ids[numComponents] = { Components::ID... };

	for (ComponentTypeID id : component_ids) {
		sig.Set(id, true);
	}

	return sig;
}

TRE_NS_END