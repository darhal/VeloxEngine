#pragma once

#include "Core/Misc/Defines/Common.hpp"
#include <Core/DataStructure/PackedArray/PackedArray.hpp>

TRE_NS_START

typedef void(*BackendDispatchFunction)(const void*);

class Texture;
class Material;
class FBO;
class RBO;
class VBO;
class VAO;
class ShaderProgram;
struct RenderTarget;

template<typename T>
struct ResourceManagerInfo
{
    typedef PackedArray<T> Container;
    typedef typename Container::INDEX_t Index;
    typedef typename Container::ID_t ID;
};

template<>
struct ResourceManagerInfo<ShaderProgram>
{
    typedef PackedArray<ShaderProgram, 256, uint16, uint8> Container;
    typedef typename Container::INDEX_t Index;
    typedef typename Container::ID_t ID;
};

template<>
struct ResourceManagerInfo<FBO>
{
    typedef PackedArray<FBO, 256, uint16, uint8> Container;
    typedef typename Container::INDEX_t Index;
    typedef typename Container::ID_t ID;
};

template<>
struct ResourceManagerInfo<RenderTarget>
{
	typedef PackedArray<RenderTarget, 256, uint16, uint8> Container;
	typedef typename Container::INDEX_t Index;
	typedef typename Container::ID_t ID;
};

template<typename T>
using RMI = ResourceManagerInfo<T>;

typedef RMI<Texture>::ID TextureID;
typedef RMI<VAO>::ID VaoID;
typedef RMI<VBO>::ID VboID;
typedef RMI<RBO>::ID RboID;
typedef RMI<FBO>::ID FboID;
typedef RMI<RenderTarget>::ID RenderTargetID;
typedef RMI<ShaderProgram>::ID ShaderID;
typedef RMI<Material>::ID MaterialID;
typedef uint16 StateHash;

struct RenderState;

template<typename T>
class RenderCommandBucket;

template<typename T>
class ResourcesCommandBucket;

template<typename T>
class FramebufferCommandBucket;

namespace RenderSettings
{
    extern RenderState DEFAULT_STATE       ;
	extern StateHash   DEFAULT_STATE_HASH  ;
    extern uint8       BLEND_DISTANCE_BITS ;
    extern FboID       DEFAULT_FRAMEBUFFER ;

    typedef RenderCommandBucket<uint64>			RenderCmdBuffer     ; 
    typedef ResourcesCommandBucket<uint8>		ResourcesCmdBuffer  ;
	typedef FramebufferCommandBucket<uint64>	FramebufferCmdBuffer;
}

TRE_NS_END