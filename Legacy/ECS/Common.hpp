#pragma once

#include <Core/Misc/Defines/Common.hpp>

TRE_NS_START

class Archetype;
class ArchetypeChunk;
class Entity;
class EntityManager;
class ECS;
struct BaseComponent;

typedef uint32 EntityID;
typedef class Entity* EntityHandle;
typedef uint32 ComponentID;
typedef uint32 ComponentTypeID;
typedef struct BaseComponent*(*ComponentCreateFunction)(uint8*, struct BaseComponent*);
typedef void(*ComponentDeleteFunction)(struct BaseComponent*);

TRE_NS_END