#pragma once

#include "Core/Misc/Defines/Common.hpp"
#include <Core/DataStructure/PackedArray/PackedArray.hpp>

TRE_NS_START

enum class ResourcesTypes
{
	VBO = 0,
	VAO = 1,
	RBO = 2,
	FBO = 3,
	TEXTURE = 4,
	MATERIAL = 5,
	SHADER = 6,
	RENDER_TARGET = 7
};

CONSTEXPR uint32 RESOURCES_COUNT = 8;

typedef void(*BackendDispatchFunction)(const void*);

class Texture;
class Material;
class FBO;
class RBO;
class VBO;
class VAO;
class ShaderProgram;
struct RenderTarget;


typedef uint16 TextureID;
typedef uint16 VaoID;
typedef uint16 VboID;
typedef uint16 RboID;
typedef uint16 FboID;
typedef uint16 RenderTargetID;
typedef uint16 ShaderID;
typedef uint16 MaterialID;

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
    extern uint8       BLEND_DISTANCE_BITS ;
    extern FboID       DEFAULT_FRAMEBUFFER ;

    typedef RenderCommandBucket<uint64>			RenderCmdBuffer     ; 
    typedef ResourcesCommandBucket<uint8>		ResourcesCmdBuffer  ;
	typedef FramebufferCommandBucket<uint64>	FramebufferCmdBuffer;
}

TRE_NS_END