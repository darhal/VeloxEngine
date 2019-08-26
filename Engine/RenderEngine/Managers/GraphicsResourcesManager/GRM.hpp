#pragma once

#include <Core/Misc/Defines/Common.hpp>
#include <Core/DataStructure/PackedArray/PackedArray.hpp>

#include <RenderAPI/VertexArray/VAO.hpp>
#include <RenderAPI/VertexBuffer/VBO.hpp>
#include <RenderAPI/FrameBuffer/FBO.hpp>
#include <RenderAPI/RenderBuffer/RBO.hpp>
#include <RenderAPI/Texture/Texture.hpp>
#include <RenderAPI/Shader/ShaderProgram.hpp>

#include <RenderEngine/Materials/Material.hpp>
#include <RenderEngine/Renderer/Common/Common.hpp>
#include <RenderEngine/Renderer/Backend/RenderTarget/RenderTarget.hpp>

TRE_NS_START

class GraphicsResourcesManager
{
public:
	GraphicsResourcesManager() {}

    // Create/Generate Function
    template<typename T, typename... Args>
    FORCEINLINE typename RMI<T>::ID Generate(Args&&... args);

    template<typename T, typename... Args>
    FORCEINLINE T* Create(typename RMI<T>::ID& id, Args&&... args);

    // Add Function
    template<typename T>
    FORCEINLINE typename RMI<T>::ID Add(T&& res);

    // Get Function
    template<typename T>
    FORCEINLINE T& Get(typename RMI<T>::ID id);

    // Remove Function
    template<typename T>
    FORCEINLINE void Remove(typename RMI<T>::ID id);

    //  Get Packed Array
    template<typename T>
    FORCEINLINE typename RMI<T>::Container& GetResourceContainer();
private:

	RMI<ShaderProgram>::Container m_Shaders;
	RMI<Material>::Container m_Materials;
    RMI<Texture>::Container m_Textures;
	
    RMI<VAO>::Container m_VAOs;
    RMI<VBO>::Container m_VBOs;

	RMI<FBO>::Container m_FBOs;
    RMI<RBO>::Container m_RBOs;
	RMI<RenderTarget>::Container m_RenderTargets;
};

using GRM = GraphicsResourcesManager;
using GraphicsResources = GraphicsResourcesManager;

#include "GRM.inl"

TRE_NS_END