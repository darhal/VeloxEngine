#pragma once

#include <Core/Misc/Defines/Common.hpp>
#include <Renderer/Common/Common.hpp>

TRE_NS_START

template<typename T>
struct ResourcesInfo
{
	CONSTEXPR static ResourcesTypes RESOURCE_TYPE;
	typedef uint32 CreationCmdType;
};


template<> struct ResourcesInfo<VBO> { CONSTEXPR static ResourcesTypes RESOURCE_TYPE = ResourcesTypes::VBO; };
template<> struct ResourcesInfo<VAO> { CONSTEXPR static ResourcesTypes RESOURCE_TYPE = ResourcesTypes::VAO; };
template<> struct ResourcesInfo<RBO> { CONSTEXPR static ResourcesTypes RESOURCE_TYPE = ResourcesTypes::RBO; };
template<> struct ResourcesInfo<FBO> { CONSTEXPR static ResourcesTypes RESOURCE_TYPE = ResourcesTypes::FBO; };
template<> struct ResourcesInfo<Texture> { CONSTEXPR static ResourcesTypes RESOURCE_TYPE = ResourcesTypes::TEXTURE; };
template<> struct ResourcesInfo<Material> { CONSTEXPR static ResourcesTypes RESOURCE_TYPE = ResourcesTypes::MATERIAL; };
template<> struct ResourcesInfo<ShaderProgram> { CONSTEXPR static ResourcesTypes RESOURCE_TYPE = ResourcesTypes::SHADER; };
template<> struct ResourcesInfo<RenderTarget> { CONSTEXPR static ResourcesTypes RESOURCE_TYPE = ResourcesTypes::RENDER_TARGET; };


TRE_NS_END
